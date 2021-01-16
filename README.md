# C-İşbirlikçi filtre ile pearson katsayısı kullanarak kullanıcılara kitap önerme | CSV dosyasından veri okuma
C - İşbirlikçi filtre ile pearson katsayısı kullanarak kullanıcılara kitap önerme sistemi

Kullanıcılara işbirlikçi filtre (collaborative filtering) ile kitap önerme
Hesaplamalar yapılırken pearson katsayısı(pearson coefficient) kullanılmıştır.

csv dosyasından okunan kullanıcıların kitaplara verdiği puanlar ile yeni eklenen kullanıcıların aralarındaki benzerlik katsayısı hesaplanır.
Baz olarak kullanılan kullanıcıların kullanıcı adları U ile başlamaktadır. Yeni kullanıcıların ise kullanıcı adları NU ile başlamaktadır.

Program başladığı zaman tüm kullanıcılar arasındaki benzerlikler hesaplanır ve NU saysısı x U sayısı boyutundaki benzerlik matrisine yazdırılır.

Daha sonra hangi kullanıcıya kitap önerileceği bilgisi kullanıcıdan alınır. Ardından NU kullanıcısı için en yakın kaç kişi arasından hesaplama yapılacağı
bilgisi sorulur. Örneğin Kullanıcı NU1 ve 3 girdisini verir ise NU1 kullanıcısına en yakın 3 U kullanıcısı hesaplanır. Ardından NU1 kullanıcısının okumadığı
kitaplar arasından kitap önerilmesi için, en çok benzeyen 3 U kullanıcısının okunmayan kitaplara verdiği puanlar dikkate alınarak, NU kullanıcısının okumadığı 
kitaplar için tahmini puanlar üretilir. Buradan en yüksek puanı alan kitap NU1 kullanıcısına önerilir.

Detaylı bilgi için Rapor.pdf dosyasını inceleyebilirsiniz.
Soru sormak için sosyal medya hesaplarıma @burakbozb1 kullanıcı adı ile ulaşabilirsiniz.
