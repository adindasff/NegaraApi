/*EL2208 Praktikum Pemecahan Masalah dengan C 2022/2023
*Modul              : 9 - Tugas Besar
*Kelompok           : E2
*Hari dan Tanggal   : Kamis, 27 April 2023
*Asisten (NIM)      : Syifa Kushirayati (18319037)
*Nama File          : main.c
*Deskripsi          :
*akan ditentukan perbatasan efektif dari wilayah negara api berdasarkan posisi markas-markas militernya. 
*Perbatasan efektif ini harus mengelilingi sebanyak mungkin markas militer, 
*dengan panjang tiap segmen garis antar markas tidak lebih dari 2500 km. 
*Program akan menampilkan markas-markas yang berada pada jalur perbatasan efektif serta yang berada di luar perbatasan efektif.
*Kemudian, panjang perbatasan efektif juga akan dihitung dan ditampilkan. 
*Permasalahan tersebut akan diselesaikan menggunakan Convex Hull Algorithm, 
*lebih spesifiknya menggunakan Gift Wrapping atau Jarvis March Algorithm.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define M_PI 3.14159265358979323846
#define RADIUS 6371 // Earth's radius in km

typedef struct Point {
    char* pelabuhan;
    double lintang;
    double bujur;
} Point;

// fungsi untuk menghitung jarak (haversine)
double calculateDistance(Point p1, Point p2) {
    //inisialisasi lintang dan bujur kedua point
    double lat1 = p1.lintang;
    double lat2 = p2.lintang;
    double lon1 = p1.bujur;
    double lon2 = p2.bujur;

    //distance antara latitudes dan longitudes, diubah dari derajat ke radian
    double dlat = (lat2 - lat1)*M_PI/180.0;
    double dlon = (lon2 - lon1)*M_PI/180.0;

    //diubah dari derajat ke radian
    lat1 = (lat1)*M_PI/180.0;
    lat2 = (lat2)*M_PI/180.0;

    //rumus atau persamaan haversine
    double a = pow(sin(dlat / 2), 2) + pow(sin(dlon / 2), 2) * cos(lat1) * cos(lat2);
    double c = 2 * asin(sqrt(a));

    return RADIUS * c;
}

// menentukan orientasi/arah dari 3 titik
int orientation(Point p, Point q, Point r) {
    double val = (q.lintang - p.lintang) * (r.bujur - q.bujur) - (r.lintang - q.lintang) * (q.bujur - p.bujur);
    if (val == 0) {
        return 0; // arah collinear (lurus)
    }
    if (val > 0)
    {
        return 1; // arah clockwise
    }
    else
    {
        return 2; // arah counterclockwise
    }
}

// membalikkan baris pada point
void swap(Point *a,Point *b) {
	Point temp=*a;*a=*b;*b=temp;
}

// mengurutkan point berdasarkan longitude (bujur) dari kecil ke besar
void sortingNilai(Point **point,int length){
    //basis
    if (length <= 1) {
        return;
    //rekurens mengurutkan nilai dari terbesar ke terkecil
    } else {
        for (int i=0; i<length-1; i++) {
            if ((*point)[i].bujur < (*point)[i+1].bujur) {
                swap(&((*point)[i]),&((*point)[i+1]));
            } 
        }
        sortingNilai(point,length-1);
    }
}

// fungsi untuk menentukan suatu titik di luar perbatasan atau tidak
int luarPerbatasan(Point* hull, int hullSize, Point p) {
    
    // convex hull tidak valid, semua titik berarti di luar
    if (hullSize < 3)
    {
        return 1; 
    }

    int i, j;
    int a = 0;

    // mengecek di luar atau tidak
    for (i = 0, j = hullSize-1; i < hullSize; j = i++) {
        int arah = (orientation(hull[i], hull[j], p));

        if (arah == 0) 
        {
            continue; // di perbatasan
        }

        if (a == 0)
        {
            a = arah;
        }
        else if (a != arah)
        {
            return 1; // di luar
        }
    }
    return 0; // di dalam
}

// fungsi untuk menentukan perbatasan efektif
void convexHull (Point **point, int n) {
    //sorting nilai bujur secara descending
    sortingNilai (point, n);

    //cek apakah minimal ada tiga titik
    if (n < 3) {
        return;
    }

    //inisialisasi point hasil
    Point *hull = (Point*)malloc(n*sizeof(Point));
    int hullSize = 0;

    //mencari titik paling kiri dari posisi
    int l = 0;
    for (int i = 1; i < n; i++) {
        if ((*point)[i].bujur < (*point)[l].bujur) {
            l = i;
        }
    }

    // inisialisasi variabel untuk convex hull
    int p = l;
    int q;
    // inisialisasi untuk total jarak perbatasan efektif
    double jarak = 0;

    // kode untuk mencari perbatasan efektif
    do {
        q = (p+1)%n;

        // menambahkan titik p ke point hull
        hull[hullSize++] = (*point)[p];

        // mencari titik paling clockwise yang jaraknya <= 2500 km dari titik p
        for (int i = 0; i < n; i++) {
            if (calculateDistance((*point)[p], (*point)[i]) <= 2500 && orientation((*point)[p],(*point)[i],(*point)[q]) == 1){
                q = i;
            }
        }

        // menghitung total jarak
        jarak = jarak + calculateDistance((*point)[p], (*point)[q]);

        // menjadikan titik tersebut sebagai titik p selanjutnya
        p = q;

    } while (p != l); // ketika kembali ke titik asal, maka loop do while ini berhenti

    // output program ditampilakn
    printf("\n");
    for (int i = 0; i < hullSize; i++) {
        printf("%s -> ", hull[i].pelabuhan);
    }
    printf("%s\n", hull[0].pelabuhan);
    printf("\nPanjang Perbatasan Efektif Negara Api: %lf km\n", jarak);
    printf("\nMarkas di Luar Perbatasan Efektif:");

    // mengeluarkan markas di luar perbatasan
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if(luarPerbatasan(hull, hullSize, (*point)[i])==1)
        {
            count = count + 1;
            printf("\n%d.%s", count, (*point)[i].pelabuhan);
        }
    }

    free(hull);
}

// fungsi untuk membuka dan mengecek file, membentuk data dari file
void openfile(char* fileName, Point **point, int *i) { 
    //membuka file
    FILE *fptr = fopen(fileName,"r");

    //mengecek ketersediaan file, jika tidak ada, akan diminta untuk input kembali hingga file ada
    while (fptr == NULL) {
        printf("Nama file tidak ditemukan! Ulangi!");
        printf("\nMasukkan file markas: ");
        scanf("%s",fileName);
        fptr = fopen(fileName,"r");
    }

    //menghitung banyak lines dari file, baris pertama tidak dihitung
    fscanf(fptr, "%*[^\n]\n");
    char line[255];
    while (fgets(line,255,fptr)) {
        (*i)++;
    }

    //membaca file dan menyimpannya ke struct point, baris pertama tidak disimpan
    rewind(fptr);
    fscanf(fptr, "%*[^\n]\n");
    *point = (Point*)malloc((*i)*sizeof(Point));
    for (int j=0; j < *i; j++) {
        (*point)[j].pelabuhan = (char*)malloc(sizeof(char));
        fscanf(fptr, "%[^,],%lf,%lf\n", (*point)[j].pelabuhan, &(*point)[j].lintang, &(*point)[j].bujur);
    }

    //menutup file
    fclose(fptr);
}

// program utama
int main() {

    // inisialisasi string untuk input nama file
    char fileName[255];

    // meminta input nama file
    printf("Masukkan file markas: ");
    scanf("%s",fileName);

    // inisialisasi struct point dari data dalam file
    struct Point *point = NULL;
    int i = 0;

    // membuka file
    openfile(fileName,&point,&i);

    // mengeluarkan perbatasan efektif
    if (i == 0) {
        printf("File kosong!");
    } else {
        printf("\nPerbatasan Efektif Negara Api:");
        convexHull(&point, i);
    }

    return 0;
}
