#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <cmath>
#include <functional>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Hamle {
    int baslangicX, baslangicY;
    int bitisX, bitisY;
    vector<pair<int,int>> yenilenTaslar;
    Hamle(int bx=0,int by=0,int ex=0,int ey=0) : baslangicX(bx), baslangicY(by), bitisX(ex), bitisY(ey) {}
};

class DamaTahtasi {
private:
    int tahta[8][8];

public:
    DamaTahtasi() {
        for(int i=0;i<8;i++) for(int j=0;j<8;j++) tahta[i][j]=0;
        for(int j=0;j<8;j++){
            tahta[1][j] = 1;
            tahta[2][j] = 1;
            tahta[5][j] = -1;
            tahta[6][j] = -1;
        }
    }

    void tahtayiGoster() {
        cout << "\n     0  1  2  3  4  5  6  7\n";
        cout << "   -------------------------\n";
        // Tahtayı ters çeviriyoruz (7'den 0'a) böylece beyaz her zaman altta
        for(int i = 7; i >= 0; i--) {
            cout << " " << i << " |";
            for(int j = 0; j < 8; j++) {
                if(tahta[i][j] == 0) cout << " . ";
                else if(tahta[i][j] == 1) cout << " b ";  // beyaz normal (küçük harf)
                else if(tahta[i][j] == 2) cout << " B ";  // beyaz dama (büyük harf)
                else if(tahta[i][j] == -1) cout << " s "; // siyah normal (küçük harf)
                else if(tahta[i][j] == -2) cout << " S "; // siyah dama (büyük harf)
            }
            cout << "| " << i << "\n";
        }
        cout << "   -------------------------\n";
        cout << "     0  1  2  3  4  5  6  7\n\n";
        cout << "b=Beyaz (Siz), s=Siyah (AI), B/S=Dama\n";
    }

    int getTas(int x,int y) const {
        if(x<0||x>=8||y<0||y>=8) return 0;
        return tahta[x][y];
    }

    void setTas(int x,int y,int v) {
        if(x<0||x>=8||y<0||y>=8) return;
        tahta[x][y]=v;
    }

    vector<Hamle> normalHamleleriGetir(int x,int y,int oyuncu) {
        vector<Hamle> hamleler;
        int tas = tahta[x][y];
        if(tas==0) return hamleler;

        bool isDama = (abs(tas)==2);
        int ileri = (tas>0) ? 1 : -1;

        if(isDama) {
            int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
            for(auto& d:dirs){
                int nx=x+d[0], ny=y+d[1];
                while(nx>=0 && nx<8 && ny>=0 && ny<8){
                    if(tahta[nx][ny]==0) hamleler.push_back(Hamle(x,y,nx,ny));
                    else break;
                    nx+=d[0]; ny+=d[1];
                }
            }
        } else {
            if(x+ileri>=0 && x+ileri<8 && tahta[x+ileri][y]==0) hamleler.push_back(Hamle(x,y,x+ileri,y));
            if(y-1>=0 && tahta[x][y-1]==0) hamleler.push_back(Hamle(x,y,x,y-1));
            if(y+1<8 && tahta[x][y+1]==0) hamleler.push_back(Hamle(x,y,x,y+1));
        }

        return hamleler;
    }

    vector<Hamle> tumYemeSekanslariniGetir(int sx,int sy) {
        vector<Hamle> sonuc;
        int tas = tahta[sx][sy];
        if(tas==0) return sonuc;
        bool isDama = (abs(tas)==2);
        int orjTas = tahta[sx][sy];
        int ileri = (tas>0) ? 1 : -1;
        vector<pair<int,int>> normDirs = { {ileri,0}, {0,-1}, {0,1} };
        vector<pair<int,int>> allDirs  = { {1,0},{-1,0},{0,1},{0,-1} };

        function<void(int,int,vector<pair<int,int>>&)> dfs2 = [&](int x,int y, vector<pair<int,int>>& curCaptured) {
            bool foundFurther = false;
            const vector<pair<int,int>>& dirs = (isDama ? allDirs : normDirs);

            for(auto d : dirs) {
                int ax = x + d.first;
                int ay = y + d.second;
                int lx = x + 2*d.first;
                int ly = y + 2*d.second;
                if(ax<0||ax>=8||ay<0||ay>=8) continue;
                if(lx<0||lx>=8||ly<0||ly>=8) continue;
                int ara = tahta[ax][ay];
                if(ara==0) continue;
                if(ara * orjTas >= 0) continue;
                if(tahta[lx][ly] != 0) continue;

                int saved_from = tahta[x][y];
                int saved_ara  = tahta[ax][ay];
                tahta[x][y] = 0;
                tahta[ax][ay] = 0;
                tahta[lx][ly] = saved_from;

                curCaptured.push_back({ax,ay});
                dfs2(lx,ly,curCaptured);

                curCaptured.pop_back();
                tahta[x][y] = saved_from;
                tahta[ax][ay] = saved_ara;
                tahta[lx][ly] = 0;

                foundFurther = true;
            }

            if(!foundFurther) {
                if(!curCaptured.empty()) {
                    Hamle h(sx,sy,x,y);
                    h.yenilenTaslar = curCaptured;
                    sonuc.push_back(h);
                }
            }
        };

        vector<pair<int,int>> temp;
        dfs2(sx,sy,temp);
        return sonuc;
    }

    vector<Hamle> gecerliHamleleriGetir(int oyuncu) {
        vector<Hamle> tumNormal;
        vector<Hamle> tumYemeler;

        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                int t = tahta[i][j];
                if(t==0) continue;
                if((oyuncu==1 && t>0) || (oyuncu==-1 && t<0)) {
                    vector<Hamle> ysek = tumYemeSekanslariniGetir(i,j);
                    tumYemeler.insert(tumYemeler.end(), ysek.begin(), ysek.end());
                }
            }
        }

        // TAŞ YEME ZORUNLU: Eğer yeme hamlesi varsa, sadece onları döndür
        if(!tumYemeler.empty()) {
            // En çok taş yiyen hamleleri bul
            size_t maxY = 0;
            for(auto &h : tumYemeler) maxY = max(maxY, h.yenilenTaslar.size());
            vector<Hamle> filtered;
            for(auto &h : tumYemeler) if(h.yenilenTaslar.size() == maxY) filtered.push_back(h);
            return filtered;
        }

        // Yeme hamlesi yoksa normal hamleleri topla
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                int t = tahta[i][j];
                if(t==0) continue;
                if((oyuncu==1 && t>0) || (oyuncu==-1 && t<0)) {
                    vector<Hamle> nors = normalHamleleriGetir(i,j,oyuncu);
                    tumNormal.insert(tumNormal.end(), nors.begin(), nors.end());
                }
            }
        }

        return tumNormal;
    }

    void hamleYap(const Hamle& h) {
        int tas = tahta[h.baslangicX][h.baslangicY];
        tahta[h.baslangicX][h.baslangicY] = 0;
        tahta[h.bitisX][h.bitisY] = tas;
        for(auto &p : h.yenilenTaslar) tahta[p.first][p.second] = 0;
        if(tas==1 && h.bitisX==7) tahta[h.bitisX][h.bitisY] = 2;
        if(tas==-1 && h.bitisX==0) tahta[h.bitisX][h.bitisY] = -2;
    }

    int taslariSay(int oyuncu) {
        int s=0;
        for(int i=0;i<8;i++) for(int j=0;j<8;j++) {
            if(oyuncu==1 && tahta[i][j]>0) s++;
            if(oyuncu==-1 && tahta[i][j]<0) s++;
        }
        return s;
    }

    bool oyunBittiMi(int oyuncu) {
        return taslariSay(oyuncu)==0 || gecerliHamleleriGetir(oyuncu).empty();
    }

    bool gayyimMi() {
        return taslariSay(1)==1 && taslariSay(-1)==1;
    }
};

class YapayZeka {
private:
    int derinlik;
    int zorlukSeviyesi; // 1=Kolay, 2=Orta, 3=Zor

    int pozisyonuDegerlendir(DamaTahtasi& tahta) {
        int skor = 0;
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                int t = tahta.getTas(i,j);
                if(t==0) continue;
                int deger = (abs(t)==1) ? 100 : 500;
                if(t>0) deger += (i) * 5;
                else deger += (7 - i) * 5;
                if(t>0) skor -= deger; else skor += deger;
            }
        }
        return skor;
    }

    int minimax(DamaTahtasi& tahta, int depth, int alpha, int beta, bool maxOyuncu) {
        if(depth==0) return pozisyonuDegerlendir(tahta);
        int oyuncu = maxOyuncu ? -1 : 1;
        vector<Hamle> hamleler = tahta.gecerliHamleleriGetir(oyuncu);
        if(hamleler.empty()) return maxOyuncu ? -100000 : 100000;

        if(maxOyuncu) {
            int maxS = numeric_limits<int>::min();
            for(auto &h : hamleler) {
                int eskiTas = tahta.getTas(h.baslangicX,h.baslangicY);
                tahta.hamleYap(h);
                int s = minimax(tahta, depth-1, alpha, beta, false);
                tahta.setTas(h.baslangicX,h.baslangicY,eskiTas);
                maxS = max(maxS, s);
                alpha = max(alpha, s);
                if(beta<=alpha) break;
            }
            return maxS;
        } else {
            int minS = numeric_limits<int>::max();
            for(auto &h : hamleler) {
                int eskiTas = tahta.getTas(h.baslangicX,h.baslangicY);
                tahta.hamleYap(h);
                int s = minimax(tahta, depth-1, alpha, beta, true);
                tahta.setTas(h.baslangicX,h.baslangicY,eskiTas);
                minS = min(minS, s);
                beta = min(beta, s);
                if(beta<=alpha) break;
            }
            return minS;
        }
    }

public:
    YapayZeka(int d=4, int zorluk=3) : derinlik(d), zorlukSeviyesi(zorluk) {}

    Hamle enIyiHamleyiBul(DamaTahtasi& tahta) {
        vector<Hamle> hamleler = tahta.gecerliHamleleriGetir(-1);
        if(hamleler.empty()) return Hamle(0,0,0,0);
        
        // ZORLUK 1: Rastgele hamle (yeme varsa rastgele yeme)
        if(zorlukSeviyesi == 1) {
            cout << "\nAI rastgele hamle seçiyor";
            cout << " Tamam!\n";
            int rastgele = rand() % hamleler.size();
            return hamleler[rastgele];
        }
        
        // ZORLUK 2: Rastgele hamle, ama yeme varsa en değerlisini yer
        if(zorlukSeviyesi == 2) {
            cout << "\nAI hamle seçiyor";
            
            // Yeme hamlesi var mı?
            bool yemeVar = false;
            for(auto &h : hamleler) {
                if(!h.yenilenTaslar.empty()) {
                    yemeVar = true;
                    break;
                }
            }
            
            if(yemeVar) {
                // En değerli taşları yiyen hamleyi bul
                int maxDeger = -1;
                vector<Hamle> enDegerliHamleler;
                
                for(auto &h : hamleler) {
                    if(h.yenilenTaslar.empty()) continue;
                    
                    int toplamDeger = 0;
                    for(auto &p : h.yenilenTaslar) {
                        int tas = tahta.getTas(p.first, p.second);
                        toplamDeger += (abs(tas) == 2) ? 500 : 100; // Dama 500, normal 100
                    }
                    
                    if(toplamDeger > maxDeger) {
                        maxDeger = toplamDeger;
                        enDegerliHamleler.clear();
                        enDegerliHamleler.push_back(h);
                    } else if(toplamDeger == maxDeger) {
                        enDegerliHamleler.push_back(h);
                    }
                }
                
                cout << " (En değerli taşı seçti) Tamam!\n";
                int rastgele = rand() % enDegerliHamleler.size();
                return enDegerliHamleler[rastgele];
            } else {
                // Yeme yoksa rastgele
                cout << " (Rastgele) Tamam!\n";
                int rastgele = rand() % hamleler.size();
                return hamleler[rastgele];
            }
        }
        
        // ZORLUK 3: Minimax ile düşünerek oyna
        Hamle best = hamleler[0];
        int bestS = numeric_limits<int>::min();
        cout << "\nAI düşünüyor";
        for(size_t i=0;i<hamleler.size();i++){
            if(i%3==0) cout << ".";
            cout.flush();
            int eskiTas = tahta.getTas(hamleler[i].baslangicX, hamleler[i].baslangicY);
            tahta.hamleYap(hamleler[i]);
            int s = minimax(tahta, derinlik-1, numeric_limits<int>::min(), numeric_limits<int>::max(), false);
            tahta.setTas(hamleler[i].baslangicX, hamleler[i].baslangicY,eskiTas);
            if(s>bestS) { bestS=s; best=hamleler[i]; }
        }
        cout << " Tamam!\n";
        return best;
    }
};

int main() {
    srand(time(0)); // Rastgele sayı üreteci için
    
    cout << "========================================\n";
    cout << "       TÜRK DAMASI - AI'YA KARŞI\n";
    cout << "========================================\n";
    cout << "*** TAŞ YEME ZORUNLUDUR! ***\n\n";
    
    cout << "İsminiz: ";
    string oyuncuIsmi;
    cin >> oyuncuIsmi;
    
    cout << "\n========================================\n";
    cout << "    AI vs " << oyuncuIsmi << "\n";
    cout << "========================================\n\n";
    
    cout << "ZORLUK SEVİYESİ SEÇİN:\n";
    cout << "1 - KOLAY (AI rastgele hamle yapar)\n";
    cout << "2 - ORTA (AI rastgele oynar ama yemede en değerlisini seçer)\n";
    cout << "3 - ZOR (AI stratejik düşünerek oynar)\n";
    cout << "\nSeçiminiz (1-3): ";
    
    string secim;
    cin >> secim;
    
    if(secim == "ecilingir") {
        cout << "\n\n🎮 Merhaba Efe! 🎮\n\n";
        return 0;
    }
    
    if(secim == "undertale") {
        cout << "\n\n";
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║                                        ║\n";
        cout << "║              GASTER                    ║\n";
        cout << "║                                        ║\n";
        cout << "║    ░█░█░█▀▄░░░█▀▀░█▀█░█▀▀░▀█▀░█▀▀░█▀▄  ║\n";
        cout << "║    ░█▄█░█░█░░░█░█░█▀█░▀▀█░░█░░█▀▀░█▀▄  ║\n";
        cout << "║    ░▀░▀░▀▀░░░░▀▀▀░▀░▀░▀▀▀░░▀░░▀▀▀░▀░▀  ║\n";
        cout << "║                                        ║\n";
        cout << "║    Beware of the man who speaks in    ║\n";
        cout << "║    hands...                            ║\n";
        cout << "║                                        ║\n";
        cout << "╚════════════════════════════════════════╝\n\n";
        return 0;
    }
    
    if(secim == "deltarune") {
        cout << "\n\n";
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║                                        ║\n";
        cout << "║              SUSIE                     ║\n";
        cout << "║                                        ║\n";
        cout << "║       ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄                  ║\n";
        cout << "║      █░░░░░░░░░░░░░░█                  ║\n";
        cout << "║      █░░▄▀▄░░░░▄▀▄░░█                  ║\n";
        cout << "║      █░░░░░░░░░░░░░░█                  ║\n";
        cout << "║      █░░░▀▀▀▀▀▀▀░░░░█                  ║\n";
        cout << "║       ▀▀▀▀▀▀▀▀▀▀▀▀▀▀                   ║\n";
        cout << "║                                        ║\n";
        cout << "║    \"Shut up and eat chalk!\"           ║\n";
        cout << "║                                        ║\n";
        cout << "╚════════════════════════════════════════╝\n\n";
        return 0;
    }
    
    if(secim == "minecraft") {
        cout << "\n\n";
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║                                        ║\n";
        cout << "║              STEVE                     ║\n";
        cout << "║                                        ║\n";
        cout << "║         ████████████████               ║\n";
        cout << "║         ████████████████               ║\n";
        cout << "║         ██░░██████░░██                 ║\n";
        cout << "║         ████████████████               ║\n";
        cout << "║         ████░░░░░░██████               ║\n";
        cout << "║         ████████████████               ║\n";
        cout << "║         ████████████████               ║\n";
        cout << "║         ██████  ██████                 ║\n";
        cout << "║         ██████  ██████                 ║\n";
        cout << "║                                        ║\n";
        cout << "║    \"Let's mine and craft!\"            ║\n";
        cout << "║                                        ║\n";
        cout << "╚════════════════════════════════════════╝\n\n";
        return 0;
    }
    
    if(secim == "ecilingirkod") {
        cout << "\n========================================\n";
        cout << "        🎯 GİZLİ KODLAR 🎯\n";
        cout << "========================================\n\n";
        cout << "ecilingir        - ???\n";
        cout << "ecilingir001     - ???\n";
        cout << "ecilingirmatrix  - ???\n";
        cout << "ecilingirstar    - ???\n";
        cout << "ecilingirrainbow - ???\n";
        cout << "ecilingirtower   - ???\n";
        cout << "ecilingircowsay  - ???\n";
        cout << "ecilingirfire    - ???\n";
        cout << "ecilingirtrain   - ???\n";
        cout << "ecilingirkod     - ???\n";
        cout << "minecraft        - ???\n";
        cout << "undertale        - ???\n";
        cout << "deltarune        - ???\n\n";
        cout << "========================================\n\n";
        return 0;
    }
    
    if(secim == "ecilingir001") {
        cout << "\n\n";
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║                                        ║\n";
        cout << "║      Merhaba " << oyuncuIsmi;
        for(int i = oyuncuIsmi.length(); i < 24; i++) cout << " ";
        cout << "║\n";
        cout << "║                                        ║\n";
        cout << "║   Hadi bunlarla vakit kaybetme        ║\n";
        cout << "║   Git dama oyna!                       ║\n";
        cout << "║                                        ║\n";
        cout << "╚════════════════════════════════════════╝\n\n";
        return 0;
    }
    
    if(secim == "ecilingirmatrix") {
        cout << "\n\n";
        cout << "█▀▄▀█ ▄▀█ ▀█▀ █▀█ █ ▀▄▀\n";
        cout << "█ ▀ █ █▀█  █  █▀▄ █ █ █\n\n";
        cout << oyuncuIsmi << ", gerçeklik bir yanılsamadır...\n";
        cout << "Ama Türk Daması gerçektir!\n";
        cout << "Oyunu seç ve kazan!\n\n";
        return 0;
    }
    
    if(secim == "ecilingirstar") {
        cout << "\n\n";
        cout << "    *  .  *    .   *  .   .  *    .\n";
        cout << "  .   *   .  *  .    *   .  *   .\n";
        cout << "*   .  *    .  *   .   *   .  *\n\n";
        cout << "Uzak bir galakside, çok uzun zaman önce...\n\n";
        cout << oyuncuIsmi << " adında genç bir oyuncu\n";
        cout << "Türk Daması'nı keşfetti ve\n";
        cout << "efsane bir şampiyon oldu!\n\n";
        cout << "Senin hikayen şimdi başlıyor...\n\n";
        return 0;
    }
    
    if(secim == "ecilingirrainbow") {
        cout << "\n\n";
        cout << "🌈🌈🌈🌈🌈🌈🌈🌈🌈🌈🌈🌈🌈\n\n";
        cout << "   Türk Daması - " << oyuncuIsmi << " Edition\n\n";
        cout << "   Kodlayan: Efe Ecingir\n";
        cout << "   Oynayan: " << oyuncuIsmi << "\n\n";
        cout << "   İyi oyunlar! 🎮\n\n";
        cout << "🌈🌈🌈🌈🌈🌈🌈🌈🌈🌈🌈🌈🌈\n\n";
        return 0;
    }
    
    if(secim == "ecilingirtower") {
        cout << "\n\n";
        cout << "        ║║║\n";
        cout << "       ═════\n";
        cout << "      ═══════\n";
        cout << "     ═════════\n";
        cout << "    ═══════════\n";
        cout << "   ═════════════\n";
        cout << "  ═══════════════\n";
        cout << "═══════════════════\n\n";
        cout << oyuncuIsmi << ", yükseklere çık!\n";
        cout << "Her hamlede daha da güçlen!\n";
        cout << "Şimdi oyuna başla!\n\n";
        return 0;
    }
    
    if(secim == "ecilingircowsay") {
        cout << "\n\n";
        cout << " ____________________________________\n";
        cout << "< Merhaba " << oyuncuIsmi << "!";
        for(int i = oyuncuIsmi.length(); i < 21; i++) cout << " ";
        cout << ">\n";
        cout << "< Türk Daması oynamaya hazır mısın? >\n";
        cout << " ------------------------------------\n";
        cout << "        \\   ^__^\n";
        cout << "         \\  (oo)\\_______\n";
        cout << "            (__)\\       )\\/\\\n";
        cout << "                ||----w |\n";
        cout << "                ||     ||\n\n";
        return 0;
    }
    
    if(secim == "ecilingirfire") {
        cout << "\n\n";
        cout << "🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥\n";
        cout << "🔥                       🔥\n";
        cout << "🔥   " << oyuncuIsmi << ", ateşli ol!";
        for(int i = oyuncuIsmi.length(); i < 10; i++) cout << " ";
        cout << "🔥\n";
        cout << "🔥                       🔥\n";
        cout << "🔥   Damayı yakacaksın!  🔥\n";
        cout << "🔥                       🔥\n";
        cout << "🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥\n\n";
        return 0;
    }
    
    if(secim == "ecilingirtrain") {
        cout << "\n\n";
        cout << "    ____                    ____\n";
        cout << "   |    |                  |    |\n";
        cout << "   |____|__________________|____|\n";
        cout << "   |____|__________________|____|\n";
        cout << "    (oo)  (oo)  (oo)  (oo)  (oo)\n\n";
        cout << "🚂 Tren hareket ediyor, " << oyuncuIsmi << "!\n";
        cout << "🚂 Hedef: ZAFER İSTASYONU\n";
        cout << "🚂 Yolculuk başlasın!\n\n";
        return 0;
    }
    
    int zorluk = atoi(secim.c_str());
    if(zorluk < 1 || zorluk > 3) zorluk = 3;
    
    cout << "\n";
    if(zorluk == 1) cout << "*** KOLAY ZORLUK SEÇİLDİ ***\n";
    else if(zorluk == 2) cout << "*** ORTA ZORLUK SEÇİLDİ ***\n";
    else cout << "*** ZOR ZORLUK SEÇİLDİ ***\n";
    
    DamaTahtasi tahta;
    YapayZeka ai(4, zorluk);
    bool oyuncuSirasi = true;

    while(true) {
        tahta.tahtayiGoster();

        if(tahta.gayyimMi()){
            cout << "\n*** GAYYIM! Berabere! ***\n";
            break;
        }

        if(oyuncuSirasi) {
            cout << "\n>>> " << oyuncuIsmi << "'NİN SIRASI <<<\n";
            if(tahta.oyunBittiMi(1)) { cout << "\n*** AI KAZANDI! ***\n"; break; }
            vector<Hamle> hamleler = tahta.gecerliHamleleriGetir(1);
            
            // Yeme hamlesi var mı kontrol et
            bool yemeVar = false;
            for(auto &h : hamleler) {
                if(!h.yenilenTaslar.empty()) {
                    yemeVar = true;
                    break;
                }
            }
            
            if(yemeVar) {
                cout << "!!! TAŞ YEME ZORUNLU - Sadece yeme hamleleri gösteriliyor !!!\n";
            }
            
            for(size_t i=0;i<hamleler.size();i++){
                cout << i << ": (" << hamleler[i].baslangicX << "," << hamleler[i].baslangicY << ") -> ("
                     << hamleler[i].bitisX << "," << hamleler[i].bitisY << ")";
                if(!hamleler[i].yenilenTaslar.empty()) cout << " [YEME x" << hamleler[i].yenilenTaslar.size() << "]";
                cout << "\n";
            }
            int secim; cout << "\nHamle numarası: "; cin >> secim;
            if(secim<0 || secim>=(int)hamleler.size()) continue;
            tahta.hamleYap(hamleler[secim]);
            oyuncuSirasi=false;
        } else {
            cout << "\n>>> AI'NIN SIRASI <<<\n";
            if(tahta.oyunBittiMi(-1)) { 
                cout << "\n*** " << oyuncuIsmi << " KAZANDI! ***\n"; 
                if(zorluk == 3) {
                    cout << "\n🏆 ZOR ZORLUKTA KAZANDIN! 🏆\n";
                    cout << "💡 İpucu: Zorluk kısmına 'ecilingirkod' yaz\n\n";
                }
                break; 
            }
            Hamle h = ai.enIyiHamleyiBul(tahta);
            cout << "AI hamle: (" << h.baslangicX << "," << h.baslangicY << ") -> (" << h.bitisX << "," << h.bitisY << ")";
            if(!h.yenilenTaslar.empty()) cout << " [YEME x" << h.yenilenTaslar.size() << "]";
            cout << "\n";
            cout << "Devam için Enter...";
            cin.ignore(numeric_limits<streamsize>::max(),'\n'); cin.get();
            tahta.hamleYap(h);
            oyuncuSirasi=true;
        }
    }
}