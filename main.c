/*
Burak Boz
18011706
10.01.2021

Kullan�c�lara i�birlik�i filtre (collaborative filtering) ile kitap �nerme
Hesaplamalar yap�l�rken pearson katsay�s�(pearson coefficient) kullan�lm��t�r.

csv dosyas�ndan okunan kullan�c�lar�n kitaplara verdi�i puanlar ile yeni eklenen kullan�c�lar�n aralar�ndaki benzerlik katsay�s� hesaplan�r.
Baz olarak kullan�lan kullan�c�lar�n kullan�c� adlar� U ile ba�lamaktad�r. Yeni kullan�c�lar�n ise kullan�c� adlar� NU ile ba�lamaktad�r.

Program ba�lad��� zaman t�m kullan�c�lar aras�ndaki benzerlikler hesaplan�r ve NU says�s� x U say�s� boyutundaki benzerlik matrisine yazd�r�l�r.

Daha sonra hangi kullan�c�ya kitap �nerilece�i bilgisi kullan�c�dan al�n�r. Ard�ndan NU kullan�c�s� i�in en yak�n ka� ki�i aras�ndan hesaplama yap�laca��
bilgisi sorulur. �rne�in Kullan�c� NU1 ve 3 girdisini verir ise NU1 kullan�c�s�na en yak�n 3 U kullan�c�s� hesaplan�r. Ard�ndan NU1 kullan�c�s�n�n okumad���
kitaplar aras�ndan kitap �nerilmesi i�in, en �ok benzeyen 3 U kullan�c�s�n�n okunmayan kitaplara verdi�i puanlar dikkate al�narak, NU kullan�c�s�n�n okumad��� 
kitaplar i�in tahmini puanlar �retilir. Buradan en y�ksek puan� alan kitap NU1 kullan�c�s�na �nerilir.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct KISI//Ki�iler
{
	char ad[10];
	int kitapPuanlari[8];
	struct KISI *next;
};

struct KITAP//Kitaplar
{
	char ad[100];
	struct KITAP *next;
};

struct KISI *kisiHead;	//U ile ba�layan ki�ilerin linkli listesinin ba�lang�� adresini tutacakt�r.
struct KISI *tKisiHead;	//NU ile ba�layan ki�ilerin linkli listesinin ba�lang�� adresini tutacakt�r.
double **simMatrix;		//Benzerlik matrisinin ba�lang�� adresini tutacakt�r.
struct KITAP *kitapHead;//Kitaplar�n sakland��� linkli listenin ba�lang�� adresini tutacakt�r.

void parse(char * ,int);	//csv dosyas�ndan veri okuyan fonksiyondur.
void kisiSonaEkle(char * , int []);	//Ki�i linkli listesinin sonuna yeni ki�i ekler.
struct KISI* kisiOlustur(char * , int []);	//Yeni bir ki�i olu�turmak i�in bellekte yer a�ar.
void kisileriYaz();	//Ki�ilerin bulundu�u listelerin yazd�r�lmas� i�in kullan�l�r.
int userSayac();	//U ile ba�layan kullan�c�lar�n adedini d�nd�r�r.
int nUserSayac();	//NU ile ba�layan kullan�c�lar�n adedini d�nd�r�r.
void sim(int , int ,struct KISI *, struct KISI *);	//Ki�iler aras�ndaki benzerlikleri hesaplar ve benzerlik matrisi olu�turur.
void simMatrisYazdir(int ,int );	//Benzerlik matrisinin ekrana yazd�r�lmas�n� sa�lar.
struct KISI *kisiGetir(char * );	//Kullan�c� ad� girilen ki�inin adresini d�nd�r�r.
int *kKisiBul(struct KISI *,int ,int );	//Benzerlik matrisindeki en benzer k adet ki�iyi bulur.
int predict(struct KISI *,int ,int * );	//NU ki�isinin okumad��� kitaplar i�in tahmini puan olu�turur ve en y�ksek puanl� kitab�n s�ras�n� d�nd�r�r.
void kitaplariYaz();	//Kitaplar linkli listesindeki elemanlar�n ekrana yaz�lmas�n� sa�lar.
void kitapSonaEkle(char * );	//Kitaplar linkli listesinin sonuna yeni bir kitap ekler.
struct KITAP* kitapOlustur(char * );	//Listeye eklenecek yeni kitap i�in bellekte yer a�ar ve aderesi d�nd�r�r.
struct KITAP* kitapGetir(int);	//S�ras� girilen kitab�n adresini d�nd�r�r.


int main()
{
	kisiHead=NULL;
	tKisiHead=NULL;
	kitapHead=NULL;
	int nUserAdet;//NU ile ba�layan kullan�c� adedini saklayacakt�r.
	int userAdet;//U ile ba�layan kullan�c� adedini saklayacakt�r.
	char kullaniciAdi[10];//hakk�nda arama yap�lacak NU kullan�c�s�n�n ad�n� saklayacakt�r.
	struct KISI *arananKisi=NULL;//kullaniciAdi girilen ki�inin adresini saklayacakt�r.
	int k;//En benzer K ki�i
	int satirNo=0;//csv den okuma yaparken okunan sat�r�n indeksini tutacakt�r.
	int i = 0,j=0;//D�ng�lerde kullan�lacak
	int menuDurum=1;//Men�de do�ru giri� yap�lana kadar kontrol sa�lamak i�in kullan�lacakt�r.
	char cikis[]="cikis";//Men�de program�n kapat�lmas�n� komutunun kontrol edilmesi i�in kullan�lacakt�r.
	int onerilecekKitapSirasi=0;//�nerilecek kitab�n s�ras�n� saklayacakt�r.
	
	FILE *dosya = fopen("RecomendationDataSet.csv","r");
	if(dosya==NULL)
	{
		perror("Dosya acilamadi.\n");
	}
	char line[1000];
	while(fgets(line,sizeof(line),dosya))
	{
		char *token;
		token=strtok(line,",");
		while(token != NULL)
		{
			int len = strlen(token);
			token[len-1]=';';
			token[len]='\0';
			parse(token,satirNo);
			token = strtok(NULL,",");
		}
		satirNo++;
	}
	//kitaplariYaz();
	//kisileriYaz();
    nUserAdet = nUserSayac();
    userAdet = userSayac();
	simMatrix=(double **)malloc(nUserAdet*sizeof(double*));//Benzerlik matrisi csv den okunan eleman say�lar�na g�re olu�turuluyor.
	if(simMatrix!=NULL)
	{
		for(i=0;i<nUserAdet;i++)
		{
			simMatrix[i]=(double *)malloc(userAdet*sizeof(double));
			if(simMatrix[i]==NULL){//E�er bellekte yer yoksa hata d�nd�r�lecek.
				printf("Tip -1 hata olustu");
				return -1;
			}
		}	
	}
    printf("Sim matrix olusturuluyor\n");
    struct KISI *tmpTestKisi = tKisiHead;
    for(i=0;i<nUserAdet;i++)
    {
    	struct KISI *tmpKisi = kisiHead;
    	for(j=0;j<userAdet;j++)
    	{
    		sim(i,j,tmpTestKisi,tmpKisi);
    		tmpKisi = tmpKisi->next;
		}
		tmpTestKisi=tmpTestKisi->next;
	}
    simMatrisYazdir(userAdet,nUserAdet);
    printf("\n\n");
    do{//Men�
    	printf("Programi kapatmak icin 'cikis' giriniz.\n");
    	printf("Kitap onerisi yapilacak kullanici adini girin (Ornek: NU1):");
    	gets(kullaniciAdi);
    	if(strcmp(kullaniciAdi,cikis)==0)//Program�n i�lem yapmadan kapat�lmas�n� sa�layakt�r.
    	{
    		menuDurum=0;
		}
		else
		{
			arananKisi=kisiGetir(kullaniciAdi);//Girilen kullan�c� ad�na ait bir kullan�c� ger�ekten varsa adresi d�necektir.
			if(arananKisi==NULL)
    		{
	    		printf("Gecersiz kullanici adi. Lutfen tekrar deneyiniz. Programi kapatmak icin 0 giriniz.\n");
			}
		}
    	
	}while(arananKisi==NULL && menuDurum==1);//Aksi durumlarda tekrardan kullan�c� ad� istenecektir.
	
	if(menuDurum!=0)//E�er men�den do�ru �ekilde ��k�ld�ysa i�lemler ger�ekle�ecektir.
	{
		printf("En yakin k kisinin bulunmasi icin k degerini giriniz:");
		scanf("%d",&k);
		int *kSiralari;
		
		
		kSiralari=kKisiBul(arananKisi,k,userAdet);//En yak�n k ki�inin indeksleri kSiralari dizisinde saklanacakt�r.
		printf("OKUNMAYAN KITAPLARA VERILEBILECEK TAHMINI PUANLAR:\n");
		onerilecekKitapSirasi = predict(arananKisi,k,kSiralari);//En y�ksek tahmini puan� alan kitap �nerilecekKitapSirasi isimli de�i�kende saklanacakt�r.
		
		struct KITAP *tmpKitapOner = kitapGetir(onerilecekKitapSirasi);//�nerilecek kitab�n adresi  linkli listeden gelir.
		printf("\nKisiye onerilen kitap = %s\n",tmpKitapOner->ad);
	}
	else
	{
		printf("Program kapatiliyor...\n");
	}
	
	
	
	
	
    
    //TEM�ZL�K | Bellekteki ayr�lan yerlerin tekrar i�letim sistemine b�rak�lmas�.
	struct KISI * tmp = kisiHead;
    while(kisiHead != NULL)
    {
    	tmp = kisiHead->next;
    	free(kisiHead);
    	kisiHead = tmp;
	}
	
	tmp = tKisiHead;
    while(tKisiHead != NULL)
    {
    	tmp = tKisiHead->next;
    	free(kisiHead);
    	tKisiHead = tmp;
	}
	
	struct KITAP *tmp2 = kitapHead;
    while(tKisiHead != NULL)
    {
    	tmp2 = kitapHead->next;
    	free(kitapHead);
    	kitapHead = tmp2;
	}
	
	for(i=0;i<nUserAdet;i++)
	{
		free(simMatrix[i]);
	}
	free(simMatrix);
	printf("Bellek temizlendi\n");
    
    return 0;
}

void parse(char * metin,int satirNo)
{
	/*
	�nputlar: 	csv den gelen bir sat�r, bu sat�r�n numaras�
	A��klama:	csv den okunan sat�r�n gerekli linkli listeye eklenmesini sa�lar.
	*/
	int cursor=0;
	char yeniMetin[1000];
	int i=0,j=0;
	while(metin[i] != '\0')
	{
		if(metin[i]==';')
		{
			if(metin[i+1]==';')
			{
				yeniMetin[cursor]=metin[i];
				cursor++;
				yeniMetin[cursor]=' ';
				cursor++;
			}
			else
			{
				yeniMetin[cursor]=metin[i];
				cursor++;
			}
		}
		else
		{
			yeniMetin[cursor]=metin[i];
			cursor++;
		}
		
		i++;
	}
	yeniMetin[cursor]='\0';
	char delim[] = ";";
	int sayac = 0;
	char *ptr = strtok(yeniMetin, delim);
	
	if(satirNo!=0)
	{
		struct KISI yeniKisi;
		while(ptr != NULL)
		{
			
			if(sayac==0)
			{
				strcpy(yeniKisi.ad,ptr);
			}
			else
			{
				if(strcmp(ptr," ")==0 || strlen(ptr)==0)
				{
					yeniKisi.kitapPuanlari[sayac-1]=0;
					
				}
				else
				{
					
					yeniKisi.kitapPuanlari[sayac-1]=atoi(ptr);
				}
			}
			ptr = strtok(NULL, delim);
			sayac++;
		}
		
		kisiSonaEkle(yeniKisi.ad,yeniKisi.kitapPuanlari);
		
	}
	else
	{
		kitapSonaEkle(yeniMetin);
		//printf("Bu bir kitaptir : %s\n",yeniMetin);
	}
	
}

struct KISI* kisiOlustur(char * kisiAd, int puanlar[8])
{
	/*
	inputlar:	ki�inin ad�, ki�inin kitaplara verdi�i puanlar
	A��klama:	Yeni bir ki�i eklemek i�in bellekte bu ki�iye yer a�ar ve bu yerin adresini d�nd�r�r.
	*/
	int i = 0;
	struct KISI* yeniKisi = (struct KISI*) malloc(sizeof(struct KISI));
	strcpy(yeniKisi->ad,kisiAd);
	for(i = 0; i<8 ; i++)
	{
		yeniKisi->kitapPuanlari[i]=puanlar[i];
	}
	yeniKisi->next=NULL;
	
	return yeniKisi;
}


void kisiSonaEkle(char * kisiAd, int puanlar[]){
	/*
	�nputlar:	ki�i ad�,ki�inin kitaplara verdi�i puanlar
	A��klama:	Yeni gelen ki�iyi ilgili linkli listenin sonuna ekler
	*/
    struct KISI* sonaEklenecek = kisiOlustur(kisiAd,puanlar);
    if(kisiAd[0]=='N')//E�er ki�i ad� N ile ba�l�yorsa NU ki�ilerinin tutuldu�u linkli listeye ekler
    {
    	if (tKisiHead == NULL)
	    {
	        tKisiHead = sonaEklenecek;
	    }
	    else
	    {	
		    struct KISI* tmp = tKisiHead;
		    while(tmp->next != NULL)
		    {
		    	tmp=tmp->next;
			}
			tmp->next=sonaEklenecek;
	    }
	}
	else if(kisiAd[0]=='U')//E�er ki�i ad� U ile ba�l�yorsa U ki�ilerinin tutuldu�u linkli listeye ekler
	{
		if (kisiHead == NULL)
	    {
	        kisiHead = sonaEklenecek;
	    }
	    else
	    {	
		    struct KISI* tmp = kisiHead;
		    while(tmp->next != NULL)
		    {
		    	tmp=tmp->next;
			}
			tmp->next=sonaEklenecek;
	    }
	}
    
	
}

void kisileriYaz()	//Ki�ilerin bulundu�u linkli listedeki verileri(Ki�i ad� ve kitapalra verdi�i puanlar) ekrana yazar.
{
	int j = 0;
	struct KISI *tmp = kisiHead;
	printf("NORMAL KULLANICILAR:\n");
	while(tmp != NULL)
	{
		printf("Kisi ad:%s\n",tmp->ad);
		for(j=0;j<8;j++)
		{
			printf("%d . kitap puani:%d\n",j,tmp->kitapPuanlari[j]);
		}
		printf("\n=====================\n");
		tmp = tmp->next;
	}
	
	tmp = tKisiHead;
	printf("TEST KULLANICILARI:\n");
		while(tmp != NULL)
	{
		printf("Kisi ad:%s\n",tmp->ad);
		for(j=0;j<8;j++)
		{
			printf("%d . kitap puani:%d\n",j,tmp->kitapPuanlari[j]);
		}
		printf("\n=====================\n");
		tmp = tmp->next;
	}
}

int userSayac()//Ka� tane U isimli kullan�c� oldu�unu d�nd�r�r.
{
	int sayac=0;
	struct KISI *tmp = kisiHead;
	while(tmp!=NULL)
	{
		sayac++;
		tmp= tmp->next;
	}
	return sayac;
}

int nUserSayac()//Ka� tane NU isimli kullan�c� oldu�unu d�nd�r�r.
{
	int sayac=0;
	struct KISI *tmp = tKisiHead;
	while(tmp!=NULL)
	{
		sayac++;
		tmp= tmp->next;
	}
	return sayac;
}

void sim(int i, int j,struct KISI *test, struct KISI *kisi)
{
	/*
	inputlar:	matris sat�r�,matris s�tunu, NU ki�inin adresi, U ki�inin adresi
	A��klama:	Bu iki ki�i aras�ndaki benzerli�i hesaplar ve benzerlik matrisinin [i][j] g�z�ne bu hesaplanan de�eri yazar.
	X = [Rap-Ra]
	Y = [Rbp-Rb]
	a=K�k(Rap-Ra)^2)
	b=K�k(Rbp-Rb)^2)
	*/
	double x=0,y=0;
	double a=0,b=0;
	int k=0,l=0;//D�ng� i�in kullan�lacak
	double rA=0,rB=0;
	int ortakSayac=0;//Ortak okunan kitap say�s�
	int aSayac=0;
	int bSayac=0;
	double sonuc=0;//Hesaplanan sonu�
	
	for(k = 0;k<8;k++)
	{
		if(test->kitapPuanlari[k]!=0)
		{
			rA+=test->kitapPuanlari[k];
			aSayac++;
			if(kisi->kitapPuanlari[k]!=0)
			{
				ortakSayac++;
			}
		}
		if(kisi->kitapPuanlari[k]!=0)
		{
			bSayac++;
			rB+=kisi->kitapPuanlari[k];
		}
	}
	rA=rA/aSayac;
	rB=rB/bSayac;
	
	for(k = 0;k<8;k++)
	{
		if(test->kitapPuanlari[k]!=0)
		{
			if(kisi->kitapPuanlari[k]!=0)
			{
				x+=((test->kitapPuanlari[k]-rA)*(kisi->kitapPuanlari[k]-rB));
				a+=pow((test->kitapPuanlari[k]-rA),2);
				b+=pow((kisi->kitapPuanlari[k]-rB),2);
			}
		}
	}
	a=sqrt(a);
	b=sqrt(b);
	
	sonuc = (x)/(a*b);
	simMatrix[i][j]=sonuc;
	printf("%s test kisisi ile %s kisi benzerligi :%lf\n",test->ad,kisi->ad,sonuc);
}


void simMatrisYazdir(int uSay,int tSay)
{
	/*
	inputlar:	U say�s�,NU say�s�
	A��klama:	Olu�turulan benzerlik matrisini ekrana yazd�r�r.
	*/
	printf("\nBENZERLIK MATRISI:\n\n");
	int i=0,j=0;
	struct KISI *tmpT = tKisiHead;
	struct KISI *tmpKisi = kisiHead;
	printf("\t");
	for(i = 0; i<uSay;i++)
	{
		printf("%s ",tmpKisi->ad);
		tmpKisi=tmpKisi->next;
	}
	printf("\n");
	
	for(i = 0 ;i<tSay;i++)
	{
		printf("%s\t",tmpT->ad);
		struct KISI *tmpK = kisiHead;
		for(j=0;j<uSay;j++)
		{
			printf("%lf ",simMatrix[i][j]);
			tmpK = tmpK->next;
		}
		printf("\n");
		tmpT = tmpT->next;
	}
}

struct KISI *kisiGetir(char * kadi)
{
	/*
	inputlar:	kullan�c� ad�
	A��klama:	Gelen kullan�c� ad�n�n adresini d�nd�r�r. Yoksa NULL d�nd�r�r.
	*/
	struct KISI *tmp = tKisiHead;
	int kontrol=0;
	while(tmp != NULL && kontrol == 0)
	{
		if(strcmp(tmp->ad,kadi)==0)
		{
			kontrol=1;
		}
		else
		{
			tmp=tmp->next;
		}
	}
	if(kontrol==1)
	{
		return tmp;
	}
	else
	{
		return NULL;
	}
}

int *kKisiBul(struct KISI *kisi,int k,int usrAdet)
{
	/*
	inputlar:	NU ki�isinin adresi, En yak�n K ki�i say�s�, U kullan�c� adedi
	A��klama:	Gelen NU ki�isine en yak�n olan k adet ki�iyi hesaplar ve bu ki�ilerin benzerlik matrisindeki indekslerinin sakland��� bir dizinin ba�lang�� adresini d�nd�r�r
	*/
	int i=0, j=0;
	float enb=-2.0;
	int sira=0;
	float tmpDizi[usrAdet];
	int kisiSirasi=0;
	int * kSiralar;
	struct KISI *tmp = tKisiHead;
	int donguKir=1;
	kSiralar = (int *) calloc(k,sizeof(int));
	while(tmp != NULL && donguKir == 1)
	{
		if(strcmp(kisi->ad,tmp->ad)==0)
		{
			donguKir=0;
		}
		else
		{
			kisiSirasi++;
			tmp=tmp->next;
		}
	}
	
	for(i = 0;i<usrAdet;i++)
	{
		tmpDizi[i]=simMatrix[kisiSirasi][i];
	}
	
	for(j=0;j<k;j++)
	{
		sira=-1;
		enb=-3.0;
		for(i=0;i<usrAdet;i++)
		{
			if(tmpDizi[i]>=enb)
			{
				enb=tmpDizi[i];
				sira=i;
			}
		}
		kSiralar[j]=sira;
		tmpDizi[sira]=-3.0;
	}
	printf("\n\n");
	printf("En Yakin Kullanicilar:\n");
	for(i=0;i<k;i++)
	{
		struct KISI *tmpYazdir = kisiHead;
		for(j=0;j<kSiralar[i];j++)
		{
			tmpYazdir=tmpYazdir->next;
		}
		printf("%s : %lf\n",tmpYazdir->ad,simMatrix[kisiSirasi][kSiralar[i]]);
	}
	
	return kSiralar;
}


int predict(struct KISI *kisi,int k,int * kSiralar)
{
	/*
	inputlar:	NU ki�isinin adresi, En yak�n K ki�i say�s�, En yak�n k ki�inin benzerlik matrisindeki indeksleri
	A��klama:	Nu ki�isinin okumad��� kitaplar� tespit eder. Bu kitaplara en yak�n K ki�inin verdi�i puanlar� tespit eder ve bu ki�ileri verdikleri puanlara
				g�re NU ki�isinin okumad��� kitaplara verebilece�i tahmini puanlar� hesaplar ve ekrana yazd�r�r. Bu okunmayan kitaplar aras�ndan en y�ksek
				puan� alan kitab�n linkli listedeki s�ras�n� d�nd�r�r.
	*/
	float rA=0;
	int i=0,j=0;
	int okunanSayisi=0;//Ortalama hesaplmak i�in kullan�lacak
	int okunMayanSayisi=0;//Okunmayan kitap say�s�
	float sonuc=0.0;//Her kitap i�in hesaplanan sonu� puan
	float pay=0.0,payda=0.0;//Sonucun hesaplanmas� i�in kullan�lacak yard�mc� de�i�kenler
	int nuSira =0;//NU ki�isinin benzerlik matrisindeki sat�r indeksi
	int kontrol =0;//NU sat�r numaras�n� hesaplamak i�in yard�mc� de�i�ken
	float enbSonuc=-300.0;//�nerilecek kitaplar aras�nda al�nan en y�ksek puan bu de�i�kende saklanacakt�r.
	int onerilecekSira = 0;//En son en y�ksek puan� alan kitab�n s�ras� bu de�i�kende saklanacakt�r.
	
	struct KISI *tmpSira = tKisiHead;
	while(tmpSira!=NULL && kontrol ==0)
	{
		if(strcmp(tmpSira->ad,kisi->ad)==0)
		{
			kontrol =1;
		}
		else
		{
			tmpSira=tmpSira->next;
			nuSira++;
		}
	}
	for(i=0;i<8;i++)
	{
		if(kisi->kitapPuanlari[i]!=0)
		{
			rA+=kisi->kitapPuanlari[i];
			okunanSayisi++;
		}
	}
	okunMayanSayisi=8-okunanSayisi;
	rA=rA/((float)okunanSayisi);
	
	
	int okunmayanSiralar[okunMayanSayisi]; // NU nun okumad��� kitaplar�n indeksleri burada saklanacak.
	int sayac=0;
	for(i=0;i<8;i++)
	{
		if(kisi->kitapPuanlari[i]==0)
		{
			okunmayanSiralar[sayac]=i;
			sayac++;
		}
	}
	
	int l=0,m=0;//D�ng� i�in ge�ici de�i�kenler
	
	for(i=0;i<okunMayanSayisi;i++)
	{
		for(j=0;j<k;j++)
		{
			struct KISI *tmpKisiPuan = kisiHead;
			int kisiSirasi =0;
			for(l =0 ;l<kSiralar[j];l++)
			{
				tmpKisiPuan=tmpKisiPuan->next;
				kisiSirasi++;
			}
			if(tmpKisiPuan->kitapPuanlari[okunmayanSiralar[i]]!=0)
			{
				float rB=0.0;
				int bOkunanSayisi=0;//B nin okudu�u kitap say�s�
				int bOkunanToplam=0;//Rb
				for(m=0;m<8;m++)
				{
					if(tmpKisiPuan->kitapPuanlari[m]!=0)
					{
						bOkunanSayisi++;
						bOkunanToplam+=tmpKisiPuan->kitapPuanlari[m];
					}
				}
				rB=(float) bOkunanToplam/bOkunanSayisi;
				pay += simMatrix[nuSira][kisiSirasi]*((tmpKisiPuan->kitapPuanlari[okunmayanSiralar[i]])-rB);
				payda += simMatrix[nuSira][kisiSirasi];
				sonuc = rA + (pay/payda);
			}
		}
		printf("%s isimli kitap icin tahmini puan : %lf\n",kitapGetir(okunmayanSiralar[i]),sonuc);
		if(sonuc>=enbSonuc)
		{
			enbSonuc=sonuc;
			onerilecekSira=okunmayanSiralar[i];
		}
	}
	return onerilecekSira;
	
}


struct KITAP* kitapOlustur(char * kitapAd)
{
	/*
	inputlar:	eklenecek kitab�n ad�
	A��klama:	Yeni eklenecek kitap i�in bellekte yer olu�turur ve adresini d�nd�r�r.
	*/
	int i = 0;
	struct KITAP* yeniKitap = (struct KITAP*) malloc(sizeof(struct KITAP));
	strcpy(yeniKitap->ad,kitapAd);
	yeniKitap->next=NULL;
	
	return yeniKitap;
}


void kitapSonaEkle(char * kitapAd){
	/*
	inputlar:	eklenecek kitab�n ad�
	A��klama:	Yeni gelen kitab� kitaplar linkli listesinin sonuna ekler
	*/
    if(strlen(kitapAd)!= 0 && strcmp(kitapAd,"USERS")!=0)
    {
    	struct KITAP* sonaEklenecek = kitapOlustur(kitapAd);
	    if (kitapHead == NULL)
		{
		    kitapHead = sonaEklenecek;
		}
		else
		{	
			struct KITAP* tmp = kitapHead;
			while(tmp->next != NULL)
			{
				tmp=tmp->next;
			}
			tmp->next=sonaEklenecek;
		}
		printf("%s isimli Kitap eklendi\n");
	}
}

void kitaplariYaz()//Kitaplar linkli listesinde bulunan t�m kitaplar� ekrana yazd�rmak i�in kullan�l�r.
{
	int j = 0;
	struct KITAP *tmp = kitapHead;
	printf("\n=====================\n");
	printf("KITAPLAR:\n");
	while(tmp != NULL)
	{
		printf("Ad:%s\n",tmp->ad);
		tmp = tmp->next;
	}
	printf("\n=====================\n");
}

struct KITAP* kitapGetir(int siraNo) 
{
	/*
	inputlar:	kitap s�ra nosu
	A��klama:	S�ra numaras� girilen kitab�n adresini d�nd�r�r.
	*/
	int i=0;
	struct KITAP *tmpKitap=kitapHead;
	for(i=0;i<siraNo;i++)
	{
		tmpKitap=tmpKitap->next;
	}
	
	return tmpKitap;
}
