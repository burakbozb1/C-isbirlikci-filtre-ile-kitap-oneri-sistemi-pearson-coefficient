/*
Burak Boz
18011706
10.01.2021

Kullanýcýlara iþbirlikçi filtre (collaborative filtering) ile kitap önerme
Hesaplamalar yapýlýrken pearson katsayýsý(pearson coefficient) kullanýlmýþtýr.

csv dosyasýndan okunan kullanýcýlarýn kitaplara verdiði puanlar ile yeni eklenen kullanýcýlarýn aralarýndaki benzerlik katsayýsý hesaplanýr.
Baz olarak kullanýlan kullanýcýlarýn kullanýcý adlarý U ile baþlamaktadýr. Yeni kullanýcýlarýn ise kullanýcý adlarý NU ile baþlamaktadýr.

Program baþladýðý zaman tüm kullanýcýlar arasýndaki benzerlikler hesaplanýr ve NU saysýsý x U sayýsý boyutundaki benzerlik matrisine yazdýrýlýr.

Daha sonra hangi kullanýcýya kitap önerileceði bilgisi kullanýcýdan alýnýr. Ardýndan NU kullanýcýsý için en yakýn kaç kiþi arasýndan hesaplama yapýlacaðý
bilgisi sorulur. Örneðin Kullanýcý NU1 ve 3 girdisini verir ise NU1 kullanýcýsýna en yakýn 3 U kullanýcýsý hesaplanýr. Ardýndan NU1 kullanýcýsýnýn okumadýðý
kitaplar arasýndan kitap önerilmesi için, en çok benzeyen 3 U kullanýcýsýnýn okunmayan kitaplara verdiði puanlar dikkate alýnarak, NU kullanýcýsýnýn okumadýðý 
kitaplar için tahmini puanlar üretilir. Buradan en yüksek puaný alan kitap NU1 kullanýcýsýna önerilir.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct KISI//Kiþiler
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

struct KISI *kisiHead;	//U ile baþlayan kiþilerin linkli listesinin baþlangýç adresini tutacaktýr.
struct KISI *tKisiHead;	//NU ile baþlayan kiþilerin linkli listesinin baþlangýç adresini tutacaktýr.
double **simMatrix;		//Benzerlik matrisinin baþlangýç adresini tutacaktýr.
struct KITAP *kitapHead;//Kitaplarýn saklandýðý linkli listenin baþlangýç adresini tutacaktýr.

void parse(char * ,int);	//csv dosyasýndan veri okuyan fonksiyondur.
void kisiSonaEkle(char * , int []);	//Kiþi linkli listesinin sonuna yeni kiþi ekler.
struct KISI* kisiOlustur(char * , int []);	//Yeni bir kiþi oluþturmak için bellekte yer açar.
void kisileriYaz();	//Kiþilerin bulunduðu listelerin yazdýrýlmasý için kullanýlýr.
int userSayac();	//U ile baþlayan kullanýcýlarýn adedini döndürür.
int nUserSayac();	//NU ile baþlayan kullanýcýlarýn adedini döndürür.
void sim(int , int ,struct KISI *, struct KISI *);	//Kiþiler arasýndaki benzerlikleri hesaplar ve benzerlik matrisi oluþturur.
void simMatrisYazdir(int ,int );	//Benzerlik matrisinin ekrana yazdýrýlmasýný saðlar.
struct KISI *kisiGetir(char * );	//Kullanýcý adý girilen kiþinin adresini döndürür.
int *kKisiBul(struct KISI *,int ,int );	//Benzerlik matrisindeki en benzer k adet kiþiyi bulur.
int predict(struct KISI *,int ,int * );	//NU kiþisinin okumadýðý kitaplar için tahmini puan oluþturur ve en yüksek puanlý kitabýn sýrasýný döndürür.
void kitaplariYaz();	//Kitaplar linkli listesindeki elemanlarýn ekrana yazýlmasýný saðlar.
void kitapSonaEkle(char * );	//Kitaplar linkli listesinin sonuna yeni bir kitap ekler.
struct KITAP* kitapOlustur(char * );	//Listeye eklenecek yeni kitap için bellekte yer açar ve aderesi döndürür.
struct KITAP* kitapGetir(int);	//Sýrasý girilen kitabýn adresini döndürür.


int main()
{
	kisiHead=NULL;
	tKisiHead=NULL;
	kitapHead=NULL;
	int nUserAdet;//NU ile baþlayan kullanýcý adedini saklayacaktýr.
	int userAdet;//U ile baþlayan kullanýcý adedini saklayacaktýr.
	char kullaniciAdi[10];//hakkýnda arama yapýlacak NU kullanýcýsýnýn adýný saklayacaktýr.
	struct KISI *arananKisi=NULL;//kullaniciAdi girilen kiþinin adresini saklayacaktýr.
	int k;//En benzer K kiþi
	int satirNo=0;//csv den okuma yaparken okunan satýrýn indeksini tutacaktýr.
	int i = 0,j=0;//Döngülerde kullanýlacak
	int menuDurum=1;//Menüde doðru giriþ yapýlana kadar kontrol saðlamak için kullanýlacaktýr.
	char cikis[]="cikis";//Menüde programýn kapatýlmasýný komutunun kontrol edilmesi için kullanýlacaktýr.
	int onerilecekKitapSirasi=0;//Önerilecek kitabýn sýrasýný saklayacaktýr.
	
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
	simMatrix=(double **)malloc(nUserAdet*sizeof(double*));//Benzerlik matrisi csv den okunan eleman sayýlarýna göre oluþturuluyor.
	if(simMatrix!=NULL)
	{
		for(i=0;i<nUserAdet;i++)
		{
			simMatrix[i]=(double *)malloc(userAdet*sizeof(double));
			if(simMatrix[i]==NULL){//Eðer bellekte yer yoksa hata döndürülecek.
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
    do{//Menü
    	printf("Programi kapatmak icin 'cikis' giriniz.\n");
    	printf("Kitap onerisi yapilacak kullanici adini girin (Ornek: NU1):");
    	gets(kullaniciAdi);
    	if(strcmp(kullaniciAdi,cikis)==0)//Programýn iþlem yapmadan kapatýlmasýný saðlayaktýr.
    	{
    		menuDurum=0;
		}
		else
		{
			arananKisi=kisiGetir(kullaniciAdi);//Girilen kullanýcý adýna ait bir kullanýcý gerçekten varsa adresi dönecektir.
			if(arananKisi==NULL)
    		{
	    		printf("Gecersiz kullanici adi. Lutfen tekrar deneyiniz. Programi kapatmak icin 0 giriniz.\n");
			}
		}
    	
	}while(arananKisi==NULL && menuDurum==1);//Aksi durumlarda tekrardan kullanýcý adý istenecektir.
	
	if(menuDurum!=0)//Eðer menüden doðru þekilde çýkýldýysa iþlemler gerçekleþecektir.
	{
		printf("En yakin k kisinin bulunmasi icin k degerini giriniz:");
		scanf("%d",&k);
		int *kSiralari;
		
		
		kSiralari=kKisiBul(arananKisi,k,userAdet);//En yakýn k kiþinin indeksleri kSiralari dizisinde saklanacaktýr.
		printf("OKUNMAYAN KITAPLARA VERILEBILECEK TAHMINI PUANLAR:\n");
		onerilecekKitapSirasi = predict(arananKisi,k,kSiralari);//En yüksek tahmini puaný alan kitap önerilecekKitapSirasi isimli deðiþkende saklanacaktýr.
		
		struct KITAP *tmpKitapOner = kitapGetir(onerilecekKitapSirasi);//Önerilecek kitabýn adresi  linkli listeden gelir.
		printf("\nKisiye onerilen kitap = %s\n",tmpKitapOner->ad);
	}
	else
	{
		printf("Program kapatiliyor...\n");
	}
	
	
	
	
	
    
    //TEMÝZLÝK | Bellekteki ayrýlan yerlerin tekrar iþletim sistemine býrakýlmasý.
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
	Ýnputlar: 	csv den gelen bir satýr, bu satýrýn numarasý
	Açýklama:	csv den okunan satýrýn gerekli linkli listeye eklenmesini saðlar.
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
	inputlar:	kiþinin adý, kiþinin kitaplara verdiði puanlar
	Açýklama:	Yeni bir kiþi eklemek için bellekte bu kiþiye yer açar ve bu yerin adresini döndürür.
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
	Ýnputlar:	kiþi adý,kiþinin kitaplara verdiði puanlar
	Açýklama:	Yeni gelen kiþiyi ilgili linkli listenin sonuna ekler
	*/
    struct KISI* sonaEklenecek = kisiOlustur(kisiAd,puanlar);
    if(kisiAd[0]=='N')//Eðer kiþi adý N ile baþlýyorsa NU kiþilerinin tutulduðu linkli listeye ekler
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
	else if(kisiAd[0]=='U')//Eðer kiþi adý U ile baþlýyorsa U kiþilerinin tutulduðu linkli listeye ekler
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

void kisileriYaz()	//Kiþilerin bulunduðu linkli listedeki verileri(Kiþi adý ve kitapalra verdiði puanlar) ekrana yazar.
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

int userSayac()//Kaç tane U isimli kullanýcý olduðunu döndürür.
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

int nUserSayac()//Kaç tane NU isimli kullanýcý olduðunu döndürür.
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
	inputlar:	matris satýrý,matris sütunu, NU kiþinin adresi, U kiþinin adresi
	Açýklama:	Bu iki kiþi arasýndaki benzerliði hesaplar ve benzerlik matrisinin [i][j] gözüne bu hesaplanan deðeri yazar.
	X = [Rap-Ra]
	Y = [Rbp-Rb]
	a=Kök(Rap-Ra)^2)
	b=Kök(Rbp-Rb)^2)
	*/
	double x=0,y=0;
	double a=0,b=0;
	int k=0,l=0;//Döngü için kullanýlacak
	double rA=0,rB=0;
	int ortakSayac=0;//Ortak okunan kitap sayýsý
	int aSayac=0;
	int bSayac=0;
	double sonuc=0;//Hesaplanan sonuç
	
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
	inputlar:	U sayýsý,NU sayýsý
	Açýklama:	Oluþturulan benzerlik matrisini ekrana yazdýrýr.
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
	inputlar:	kullanýcý adý
	Açýklama:	Gelen kullanýcý adýnýn adresini döndürür. Yoksa NULL döndürür.
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
	inputlar:	NU kiþisinin adresi, En yakýn K kiþi sayýsý, U kullanýcý adedi
	Açýklama:	Gelen NU kiþisine en yakýn olan k adet kiþiyi hesaplar ve bu kiþilerin benzerlik matrisindeki indekslerinin saklandýðý bir dizinin baþlangýç adresini döndürür
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
	inputlar:	NU kiþisinin adresi, En yakýn K kiþi sayýsý, En yakýn k kiþinin benzerlik matrisindeki indeksleri
	Açýklama:	Nu kiþisinin okumadýðý kitaplarý tespit eder. Bu kitaplara en yakýn K kiþinin verdiði puanlarý tespit eder ve bu kiþileri verdikleri puanlara
				göre NU kiþisinin okumadýðý kitaplara verebileceði tahmini puanlarý hesaplar ve ekrana yazdýrýr. Bu okunmayan kitaplar arasýndan en yüksek
				puaný alan kitabýn linkli listedeki sýrasýný döndürür.
	*/
	float rA=0;
	int i=0,j=0;
	int okunanSayisi=0;//Ortalama hesaplmak için kullanýlacak
	int okunMayanSayisi=0;//Okunmayan kitap sayýsý
	float sonuc=0.0;//Her kitap için hesaplanan sonuç puan
	float pay=0.0,payda=0.0;//Sonucun hesaplanmasý için kullanýlacak yardýmcý deðiþkenler
	int nuSira =0;//NU kiþisinin benzerlik matrisindeki satýr indeksi
	int kontrol =0;//NU satýr numarasýný hesaplamak için yardýmcý deðiþken
	float enbSonuc=-300.0;//Önerilecek kitaplar arasýnda alýnan en yüksek puan bu deðiþkende saklanacaktýr.
	int onerilecekSira = 0;//En son en yüksek puaný alan kitabýn sýrasý bu deðiþkende saklanacaktýr.
	
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
	
	
	int okunmayanSiralar[okunMayanSayisi]; // NU nun okumadýðý kitaplarýn indeksleri burada saklanacak.
	int sayac=0;
	for(i=0;i<8;i++)
	{
		if(kisi->kitapPuanlari[i]==0)
		{
			okunmayanSiralar[sayac]=i;
			sayac++;
		}
	}
	
	int l=0,m=0;//Döngü için geçici deðiþkenler
	
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
				int bOkunanSayisi=0;//B nin okuduðu kitap sayýsý
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
	inputlar:	eklenecek kitabýn adý
	Açýklama:	Yeni eklenecek kitap için bellekte yer oluþturur ve adresini döndürür.
	*/
	int i = 0;
	struct KITAP* yeniKitap = (struct KITAP*) malloc(sizeof(struct KITAP));
	strcpy(yeniKitap->ad,kitapAd);
	yeniKitap->next=NULL;
	
	return yeniKitap;
}


void kitapSonaEkle(char * kitapAd){
	/*
	inputlar:	eklenecek kitabýn adý
	Açýklama:	Yeni gelen kitabý kitaplar linkli listesinin sonuna ekler
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

void kitaplariYaz()//Kitaplar linkli listesinde bulunan tüm kitaplarý ekrana yazdýrmak için kullanýlýr.
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
	inputlar:	kitap sýra nosu
	Açýklama:	Sýra numarasý girilen kitabýn adresini döndürür.
	*/
	int i=0;
	struct KITAP *tmpKitap=kitapHead;
	for(i=0;i<siraNo;i++)
	{
		tmpKitap=tmpKitap->next;
	}
	
	return tmpKitap;
}
