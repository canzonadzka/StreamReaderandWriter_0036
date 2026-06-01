#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <stdexcept>
#include <limits>
#include <iomanip>

using namespace std;

// -- Struct
struct Produk {
    string kode, nama, kategori;
    int stok;
    double harga;
};

// -- Helper
string toLine(const Produk& p) {
    return p.kode + "|" + p.nama + "|" + p.kategori + "|"
         + to_string(p.stok) + "|" + to_string(p.harga);
}

Produk fromLine(const string& baris) {
    Produk p;
    string tok;
    auto it = baris.begin();
    auto next = [&](char delim) {
        tok.clear();
        while (it != baris.end() && *it != delim) tok += *it++;
        if (it != baris.end()) ++it;
        return tok;
    };
    p.kode     = next('|');
    p.nama     = next('|');
    p.kategori = next('|');
    p.stok     = stoi(next('|'));
    p.harga    = stod(next('|'));
    return p;
}

// -- Class TokoElektronik
class TokoElektronik {
private:
    array<Produk, 3> etalase;

public:
    TokoElektronik() {
        etalase[0] = {"ETL-01", "Samsung QLED 55\"", "TV",         5, 12500000};
        etalase[1] = {"ETL-02", "iPhone 15 Pro Max", "Smartphone", 3, 21000000};
        etalase[2] = {"ETL-03", "Sony WH-1000XM5",  "Headphone",  8,  4750000};
    }

    Produk ambil(size_t nomorRak) {
        try {
            return etalase.at(nomorRak);
        } catch (const out_of_range&) {
            throw runtime_error(
                "Gagal Mengambil Barang : Rak nomor " +
                to_string(nomorRak) + " kosong atau tidak tersedia!"
            );
        }
    }

    void tampilEtalase() {
        cout << "\n--- ETALASE ---\n";
        for (size_t i = 0; i < etalase.size(); i++)
            cout << "[" << i << "] " << etalase[i].kode
                 << " | " << etalase[i].nama
                 << " | Stok: " << etalase[i].stok
                 << " | Rp " << fixed << setprecision(0) << etalase[i].harga << "\n";
    }
};

// -- Class GudangManager
class GudangManager {
private:
    string file;

    vector<Produk> bacaSemua() {
        vector<Produk> list;
        ifstream f(file);
        string baris;
        while (getline(f, baris))
            if (!baris.empty()) list.push_back(fromLine(baris));
        return list;
    }

    void tulisSemua(const vector<Produk>& list) {
        ofstream f(file, ios::trunc);
        for (const auto& p : list) f << toLine(p) << "\n";
    }

public:
    GudangManager(const string& namaFile) : file(namaFile) {}

    void tampil() {
        auto list = bacaSemua();
        cout << "\n=== DAFTAR GUDANG ===\n";
        if (list.empty()) { cout << "[Gudang kosong]\n"; return; }
        int no = 1;
        for (const auto& p : list)
            cout << no++ << ". [" << p.kode << "] " << p.nama
                 << " | " << p.kategori
                 << " | Stok: " << p.stok
                 << " | Rp " << fixed << setprecision(0) << p.harga << "\n";
    }

    void tambah() {
        Produk p;
        auto list = bacaSemua();
        cout << "\n-- TAMBAH PRODUK --\n";

        bool duplikat;
        do {
            cout << "Kode    : "; cin >> p.kode; cin.ignore();
            duplikat = false;
            for (const auto& x : list)
                if (x.kode == p.kode) { duplikat = true; break; }
            if (duplikat) cout << "[!] Kode sudah ada.\n";
        } while (duplikat);

        cout << "Nama    : "; getline(cin, p.nama);
        cout << "Kategori: "; getline(cin, p.kategori);
        cout << "Stok    : "; cin >> p.stok;
        cout << "Harga   : "; cin >> p.harga;
        cin.ignore();

        ofstream f(file, ios::app);
        f << toLine(p) << "\n";
        cout << "[OK] Produk ditambahkan.\n";
    }

    void update() {
        tampil();
        auto list = bacaSemua();
        if (list.empty()) return;

        string kode;
        cout << "\n-- UPDATE PRODUK --\nKode: "; cin >> kode; cin.ignore();

        bool ada = false;
        for (auto& p : list) {
            if (p.kode == kode) {
                ada = true;
                cout << "Nama baru    : "; getline(cin, p.nama);
                cout << "Kategori baru: "; getline(cin, p.kategori);
                cout << "Stok baru    : "; cin >> p.stok;
                cout << "Harga baru   : "; cin >> p.harga;
                cin.ignore();
                break;
            }
        }
        if (!ada) { cout << "[!] Kode tidak ditemukan.\n"; return; }

        tulisSemua(list);
        cout << "[OK] Data diperbarui.\n";
    }

    void hapus() {
        tampil();
        auto list = bacaSemua();
        if (list.empty()) return;

        string kode, konfirm;
        cout << "\n-- HAPUS PRODUK --\nKode: "; cin >> kode; cin.ignore();

        vector<Produk> baru;
        bool ada = false;
        for (const auto& p : list) {
            if (p.kode == kode) {
                ada = true;
                cout << "Hapus \"" << p.nama << "\"? (y/n): ";
                getline(cin, konfirm);
                if (konfirm == "y" || konfirm == "Y")
                    cout << "[OK] Produk dihapus.\n";
                else
                    baru.push_back(p);
            } else {
                baru.push_back(p);
            }
        }
        if (!ada) cout << "[!] Kode tidak ditemukan.\n";
        else tulisSemua(baru);
    }
};

// -- Simulasi etalase
void simulasiEtalase(TokoElektronik& toko) {
    toko.tampilEtalase();

    cout << "\n[Skenario 1] Ambil rak indeks 1:\n";
    try {
        Produk p = toko.ambil(1);
        cout << "[SUKSES] " << p.nama << " | Rp " << fixed << setprecision(0) << p.harga << "\n";
    } catch (const runtime_error& e) {
        cout << "[ERROR] " << e.what() << "\n";
    }

    cout << "\n[Skenario 2] Ambil rak indeks 5:\n";
    try {
        Produk p = toko.ambil(5);
        cout << "[SUKSES] " << p.nama << "\n";
    } catch (const runtime_error& e) {
        cout << "[ERROR] " << e.what() << "\n";
        cout << ">> Program tetap berjalan aman.\n";
    }
}

// -- Main
int main() {
    TokoElektronik toko;
    GudangManager gudang("gudang.txt");
    int pilih;

    cout << "=== TOKO ELEKTRONIK GIBRAN JAYA ===\n";

    do {
        gudang.tampil();
        cout << "\n[1] Tambah  [2] Update  [3] Hapus  [4] Simulasi Etalase  [0] Keluar\n";
        cout << "Pilih: "; cin >> pilih;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[!] Input tidak valid.\n";
            continue;
        }
        cin.ignore();

        switch (pilih) {
            case 1: gudang.tambah();              break;
            case 2: gudang.update();              break;
            case 3: gudang.hapus();               break;
            case 4: simulasiEtalase(toko);        break;
            case 0: cout << "Program selesai.\n"; break;
            default: cout << "[!] Pilihan tidak ada.\n";
        }

        if (pilih != 0) {
            cout << "\nEnter untuk lanjut...";
            cin.get();
        }

    } while (pilih != 0);

    return 0;
}