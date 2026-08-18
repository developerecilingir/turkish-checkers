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

        for(int i = 7; i >= 0; i--) {
            cout << " " << i << " |";
            for(int j = 0; j < 8; j++) {
                if(tahta[i][j] == 0) cout << " . ";
                else if(tahta[i][j] == 1) cout << " b ";
                else if(tahta[i][j] == 2) cout << " B ";
                else if(tahta[i][j] == -1) cout << " s ";
                else if(tahta[i][j] == -2) cout << " S ";
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

    void yemeHamleleriniBul(int x, int y, int oyuncu, vector<pair<int,int>>& yenilen, vector<Hamle>& sonuc, Hamle mevcut) {
        int tas = tahta[x][y];
        bool isDama = (abs(tas)==2);
        int yonler[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};

        for(auto& yon : yonler) {
            if(isDama) {
                int nx = x + yon[0], ny = y + yon[1];
                while(nx>=0 && nx<8 && ny>=0 && ny<8 && tahta[nx][ny]==0) {
                    nx += yon[0]; ny += yon[1];
                }
                if(nx>=0 && nx<8 && ny>=0 && ny<8 && tahta[nx][ny]*oyuncu < 0) {
                    int rakipX = nx, rakipY = ny;
                    nx += yon[0]; ny += yon[1];
                    while(nx>=0 && nx<8 && ny>=0 && ny<8 && tahta[nx][ny]==0) {
                        bool zatenYenildi = false;
                        for(auto& p : yenilen) if(p.first==rakipX && p.second==rakipY) zatenYenildi=true;
                        if(!zatenYenildi) {
                            Hamle yeni = mevcut;
                            yeni.bitisX = nx; yeni.bitisY = ny;
                            yeni.yenilenTaslar = yenilen;
                            yeni.yenilenTaslar.push_back({rakipX, rakipY});
                            sonuc.push_back(yeni);
                            int eski = tahta[rakipX][rakipY];
                            tahta[rakipX][rakipY] = 0;
                            tahta[x][y] = 0;
                            tahta[nx][ny] = tas;
                            yemeHamleleriniBul(nx, ny, oyuncu, yeni.yenilenTaslar, sonuc, yeni);
                            tahta[rakipX][rakipY] = eski;
                            tahta[x][y] = tas;
                            tahta[nx][ny] = 0;
                        }
                        nx += yon[0]; ny += yon[1];
                    }
                }
            } else {
                int nx = x + yon[0], ny = y + yon[1];
                int nnx = nx + yon[0], nny = ny + yon[1];
                if(nx>=0 && nx<8 && ny>=0 && ny<8 && nnx>=0 && nnx<8 && nny>=0 && nny<8) {
                    if(tahta[nx][ny]*oyuncu < 0 && tahta[nnx][nny]==0) {
                        bool zatenYenildi = false;
                        for(auto& p : yenilen) if(p.first==nx && p.second==ny) zatenYenildi=true;
                        if(!zatenYenildi) {
                            Hamle yeni = mevcut;
                            yeni.bitisX = nnx; yeni.bitisY = nny;
                            yeni.yenilenTaslar = yenilen;
                            yeni.yenilenTaslar.push_back({nx, ny});
                            sonuc.push_back(yeni);
                            int eski = tahta[nx][ny];
                            tahta[nx][ny] = 0;
                            tahta[x][y] = 0;
                            tahta[nnx][nny] = tas;
                            yemeHamleleriniBul(nnx, nny, oyuncu, yeni.yenilenTaslar, sonuc, yeni);
                            tahta[nx][ny] = eski;
                            tahta[x][y] = tas;
                            tahta[nnx][nny] = 0;
                        }
                    }
                }
            }
        }
    }

    vector<Hamle> yemeHamleleriGetir(int x, int y, int oyuncu) {
        vector<Hamle> sonuc;
        vector<pair<int,int>> bos;
        Hamle baslangic(x,y,x,y);
        yemeHamleleriniBul(x, y, oyuncu, bos, sonuc, baslangic);
        return sonuc;
    }

    vector<Hamle> gecerliHamleleriGetir(int oyuncu) {
        vector<Hamle> tum;
        vector<Hamle> yemeler;
        for(int i=0;i<8;i++) for(int j=0;j<8;j++) {
            if(tahta[i][j]*oyuncu > 0) {
                auto y = yemeHamleleriGetir(i,j,oyuncu);
                yemeler.insert(yemeler.end(), y.begin(), y.end());
                auto n = normalHamleleriGetir(i,j,oyuncu);
                tum.insert(tum.end(), n.begin(), n.end());
            }
        }
        if(!yemeler.empty()) return yemeler;
        return tum;
    }

    void hamleYap(const Hamle& h) {
        int tas = tahta[h.baslangicX][h.baslangicY];
        tahta[h.baslangicX][h.baslangicY] = 0;
        for(auto& p : h.yenilenTaslar) tahta[p.first][p.second] = 0;
        tahta[h.bitisX][h.bitisY] = tas;
        if(tas==1 && h.bitisX==7) tahta[h.bitisX][h.bitisY]=2;
        if(tas==-1 && h.bitisX==0) tahta[h.bitisX][h.bitisY]=-2;
    }

    bool oyunBittiMi(int oyuncu) {
        return gecerliHamleleriGetir(oyuncu).empty();
    }

    bool gayyimMi() {
        int beyaz=0, siyah=0;
        for(int i=0;i<8;i++) for(int j=0;j<8;j++) {
            if(tahta[i][j]>0) beyaz++;
            if(tahta[i][j]<0) siyah++;
        }
        return beyaz==1 && siyah==1;
    }

    int degerlendir() {
        int skor=0;
        for(int i=0;i<8;i++) for(int j=0;j<8;j++) {
            int t = tahta[i][j];
            if(t==1) skor += 10;
            else if(t==2) skor += 30;
            else if(t==-1) skor -= 10;
            else if(t==-2) skor -= 30;
            if(t>0) skor += (i);
            if(t<0) skor -= (7-i);
        }
        return skor;
    }
};

class YapayZeka {
    int derinlik;
    int zorluk;
public:
    YapayZeka(int d=4, int z=2) : derinlik(d), zorluk(z) {}

    int minimax(DamaTahtasi& tahta, int depth, int alpha, int beta, bool maksimize) {
        if(depth==0 || tahta.oyunBittiMi(1) || tahta.oyunBittiMi(-1)) return tahta.degerlendir();

        if(maksimize) {
            int maxEval = numeric_limits<int>::min();
            auto hamleler = tahta.gecerliHamleleriGetir(-1);
            for(auto& h : hamleler) {
                DamaTahtasi kopya = tahta;
                kopya.hamleYap(h);
                int eval = minimax(kopya, depth-1, alpha, beta, false);
                maxEval = max(maxEval, eval);
                alpha = max(alpha, eval);
                if(beta <= alpha) break;
            }
            return maxEval;
        } else {
            int minEval = numeric_limits<int>::max();
            auto hamleler = tahta.gecerliHamleleriGetir(1);
            for(auto& h : hamleler) {
                DamaTahtasi kopya = tahta;
                kopya.hamleYap(h);
                int eval = minimax(kopya, depth-1, alpha, beta, true);
                minEval = min(minEval, eval);
                beta = min(beta, eval);
                if(beta <= alpha) break;
            }
            return minEval;
        }
    }

    Hamle enIyiHamleyiBul(DamaTahtasi& tahta) {
        auto hamleler = tahta.gecerliHamleleriGetir(-1);
        if(hamleler.empty()) return Hamle();

        if(zorluk==1) {
            return hamleler[rand() % hamleler.size()];
        }

        Hamle enIyi;
        int enIyiSkor = numeric_limits<int>::min();
        int d = (zorluk==3) ? derinlik+1 : derinlik;

        for(auto& h : hamleler) {
            DamaTahtasi kopya = tahta;
            kopya.hamleYap(h);
            int skor = minimax(kopya, d-1, numeric_limits<int>::min(), numeric_limits<int>::max(), false);
            if(skor > enIyiSkor) {
                enIyiSkor = skor;
                enIyi = h;
            }
        }
        return enIyi;
    }
};

int main() {
    srand(time(0));
    cout << "=== TURKISH CHECKERS (DAMA) ===\n";
    cout << "Oyuncu ismi: ";
    string oyuncuIsmi;
    cin >> oyuncuIsmi;

    cout << "Zorluk (1=Kolay, 2=Orta, 3=Zor): ";
    int zorluk;
    cin >> zorluk;
    if(zorluk < 1 || zorluk > 3) zorluk = 2;

    string kod;
    if(zorluk == 3) {
        cout << "Kod gir (bos birakabilirsin): ";
        cin.ignore();
        getline(cin, kod);
        if(kod == "ecilingirkod") {
            cout << "*** GIZLI MOD AKTIF ***\n";
        }
    }

    if(zorluk == 1) cout << "*** KOLAY ZORLUK SECILDI ***\n";
    else if(zorluk == 2) cout << "*** ORTA ZORLUK SECILDI ***\n";
    else cout << "*** ZOR ZORLUK SECILDI ***\n";

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
            cout << "\n>>> " << oyuncuIsmi << "'NIN SIRASI <<<\n";
            if(tahta.oyunBittiMi(1)) { cout << "\n*** AI KAZANDI! ***\n"; break; }
            vector<Hamle> hamleler = tahta.gecerliHamleleriGetir(1);

            bool yemeVar = false;
            for(auto &h : hamleler) {
                if(!h.yenilenTaslar.empty()) {
                    yemeVar = true;
                    break;
                }
            }

            if(yemeVar) {
                cout << "!!! TAS YEME ZORUNLU - Sadece yeme hamleleri gosteriliyor !!!\n";
            }

            for(size_t i=0;i<hamleler.size();i++){
                cout << i << ": (" << hamleler[i].baslangicX << "," << hamleler[i].baslangicY << ") -> ("
                     << hamleler[i].bitisX << "," << hamleler[i].bitisY << ")";
                if(!hamleler[i].yenilenTaslar.empty()) cout << " [YEME x" << hamleler[i].yenilenTaslar.size() << "]";
                cout << "\n";
            }
            int secim; cout << "\nHamle numarasi: "; cin >> secim;
            if(secim<0 || secim>=(int)hamleler.size()) continue;
            tahta.hamleYap(hamleler[secim]);
            oyuncuSirasi=false;
        } else {
            cout << "\n>>> AI'NIN SIRASI <<<\n";
            if(tahta.oyunBittiMi(-1)) {
                cout << "\n*** " << oyuncuIsmi << " KAZANDI! ***\n";
                if(zorluk == 3) {
                    cout << "\n🏆 ZOR ZORLUKTA KAZANDIN! 🏆\n";
                    cout << "💡 Ipucu: Zorluk kismina 'ecilingirkod' yaz\n\n";
                }
                break;
            }
            Hamle h = ai.enIyiHamleyiBul(tahta);
            cout << "AI hamle: (" << h.baslangicX << "," << h.baslangicY << ") -> (" << h.bitisX << "," << h.bitisY << ")";
            if(!h.yenilenTaslar.empty()) cout << " [YEME x" << h.yenilenTaslar.size() << "]";
            cout << "\n";
            cout << "Devam icin Enter...";
            cin.ignore(numeric_limits<streamsize>::max(),'\n'); cin.get();
            tahta.hamleYap(h);
            oyuncuSirasi=true;
        }
    }
}
