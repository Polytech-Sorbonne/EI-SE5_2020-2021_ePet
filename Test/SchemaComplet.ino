// ############################ Librairies ############################

// Librairies Accéléromètre
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

// Librairies Réseaux Wifi
#include "WiFi.h"
#include <HardwareSerial.h>

//Librairies Température
#include "Adafruit_SHTC3.h"


// ############################ Macros ############################

#define DEBUG true
#define RX0 16 // PIN RX0 <=> D3
#define TX0 17 // PIN TX0 <=> D1
#define NB_SECOND 60/15 // 1 min
#define DELAY_TASK 60000 / 15

//Accelerometre
#define SEUILX 1.3 
#define SEUILY 1.3

//seuils sur X et Y accumulés pendant 15 minutes_acc
#define Xaddseuil 10
#define Yaddseuil 10

// ############################ Variables ############################

Adafruit_MMA8451 mma = Adafruit_MMA8451();

// Message SigFox
byte bssid1_prec[6], bssid2_prec[6], bssid1[6], bssid2[6];
byte rssi1, rssi2; 

// Température
int t, temp1, temp2;
//tableaux des températures sur 15 minutes_acc
int temp_tab[7];

hw_timer_t * timer;
volatile int interruptCounter;
volatile int minutes_acc;
int totalInterruptCounter;
sensors_event_t humidity, temp;
sensors_event_t event;

double x_0;
double x_1;
double y_0;
double y_1;

double diffX = 0;
double diffY = 0;
int countX = 0 ,countY = 0;
int countmintX = 0, countmintY = 0;
int addedcountX = 0, addedcountY = 0; 

//tableaux des mouvements sur les deux axes sur 15 minutes_acc 
int mouvX[15];
int mouvY[15];

String status = "";
char output;

char *mac[] = {
  "9424E1","A09D86","C8F86D","00118B","00E0B1","00E0DA","DC0856","54E3F6","1880F5","000F62","184A6F","84A783","F83094","68597F","0025BA","00153F","002032","00C0BE","2CFAA2","E8E732",
  "A8AD3D","24AF4A","7C2064","D099D5","9067F3","18422F","0017CC","008039","3C28A6","0CB5DE","002060","54A619","002105","D4224E","94AE61","4C5FD2","4CA74B","54055F","9067B5","001D4C",
  "00A081","008021","6854ED","3C8BCD","00164D","001AF0","AC9CE4","00233E","6CBEE9","0021AE","001C8E","00113F","00089A","00D095","0020DA","000772","E03005","0CA402","A0F3E4","E4A1E6",
  "F4C613","885C47","040A83","002135","000E86","DC446D","608B0E","88B291","C42AD0","CCD281","1040F3","586B14","BCB863","44E66E","C0E862","F40616","E4B2FB","4C569D","38539C","402619",
  "6CE85C","F83880","1094BB","F86FC1","28FF3C","F099B6","88E9FE","38892C","749EAF","94BF2D","40CBC0","C4618B","08E689","DC56E7","5433CB","3408BC","1C36BB","3C2EFF","24F677","B0CA68",
  "C83C85","B8634D","A4E975","3035AD","844167","9800C6","AC1F74","A85C2C","00DB70","245BA7","70F087","B0702D","6C19C0","2C3361","ECADB8","9801A7","24F094","086D41","E0C767","80ED2C",
  "AC61EA","38B54D","1C5CF2","28ED6A","A4D18C","241EEB","CC25EF","28CFE9","00A040","3CD0F8","680927","6CC26B","44D884","002608","0026B0","0026BB","D49A20","F81EDF","C82A14","3C0754",
  "A4B197","F0B479","1093E9","442A60","A4D1D2","28CFDA","003065","001451","001E52","0021E9","CC08E0","045453","F4F951","C06394","18AF8F","C8B5B7","90B21F","30F7C5","40B395","44FB42",
  "E88D28","949426","207D74","F4F15A","C86F1D","3090AB","8C2DAA","848506","98FE94","D8004D","64200C","C8334B","64E682","B8E856","D89695","1499E2","B418D1","9C207B","B065BD","542696",
  "64A3CB","903C92","D81D72","341298","70E72C","70ECE4","68AE20","AC87A3","D8BB2C","D04F7E","2078F0","E0ACCB","A0999B","24240E","F0DBF8","48746E","54AE27","FCE998","0CBC9F","34363B",
  "D0A637","789F70","9CF387","A85B78","C8F650","A88E24","CC20E8","70480F","F0B0E7","0469F8","4070F5","B035B5","800C67","90812A","B87BC5","102959","E47684","F05CD5","70EA5A","B440A4",
  "48B8A3","F4DBE3","E8854B","28EC95","E02B96","FC66CF","AC1D06","44A8FC","F81093","B456E3","4C20B8","1488E6","B485E1","0C19F8","501FC6","540910","9CFC28","CC69FA","10CEE9","FC4EA4",
  "F4BEEC","54E61B","F887F1","305714","C8B1CD","1460CB","B8F12A","804A14","703C69","14C213","A4D931","BCFED9","808223","BCE143","647033","846878","C8D083","6030D4","F895EA","18F1D8",
  "30D9D9","D0817A","98CA33","68AB1E","70EF00","0C1539","D88F76","409C28","58E28F","787B8A","503237","B0481A","B49CDF","48BF6B","9C84BF","00B362","E4E4AB","60334B","FCD848","A860B6",
  "C4B301","E05F45","483B38","1C9148","30636B","A4F1E8","440010","0056CD","00CDFE","E498D6","F431C3","64A5C3","002332","00236C","0023DF","002500","0025BC","0019E3","001B63","001EC2",
  "001FF3","0010FA","0050E4","000D93","7CFADF","78A3E4","148FC6","286AB8","28E02C","E0B9BA","00C610","B8F6B1","8CFABA","7CD1C3","F0DCE2","24AB81","E0F847","28E7CF","E4CE8F","A82066",
  "BC52B7","5C5948","C8BCC8","E8040B","145A05","1CABA7","C0847A","34159E","58B035","DC86D8","90B931","D0E140","24A2E1","80EA96","600308","04F13E","98F0AB","7831C1","783A84","5C8D4E",
  "8863DF","881FA1","C8E0EB","98B8E3","885395","786C1C","4C8D79","1CE62B","0C3021","0C3E9F","FCFC48","9C293F","087402","94F6A3","98E0D9","CC29F5","285AEB","F02475","2C1F23","549F13",
  "F0DBE2","748114","18F643","A45E60","A01828","D0034B","10417F","A8667F","D02598","80BE05","24A074","84788B","587F57","006D52","B844D9","14876A","E0B55F","F8FFC2","E0EB40","3CBF60",
  "AC15F4","88A479","047295","D446E1","78D162","08F8BC","90A25B","7CAB60","44C65D","187EB9","20E874","D03FAA","640BD7","A8913D","0C3B50","748F3C","40F946","B41BB0","58D349","F434F0",
  "B08C75","9C28B3","A07817","5C8730","14D19E","40C711","5C7017","8CEC7B","B881FA","9C760E","94EA32","50F4EB","28C709","507AC5","4C6BE8","8C861E","542B8D","4418FD","005B94","E0897E",
  "64C753","38F9D3","FC183C","90E17B","D81C79","58E6BA","9CE65E","C49880","E0338E","08F69C","50A67F","D461DA","F01898","881908","5C0947","14205E","B841A4","D4909C","E4E0A6","80B03D",
  "E49ADC","ACE4B5","D0D2B0","8C8590","6C96CF","78886D","20EE28","B4F61C","08F4AB","D4DCCD","484BAA","DCA904","6CAB31","4C74BF","989E63","886B6E","5082D5","64B0A6","7C04D0","84FCAC",
  "DC0C5C","70700D","186590","F86214","784F43","404D7F","9C8BA0","CC088D","609AC1","748D08","10DDB1","C01ADA","68FB7E","84A134","0C5101","2CF0A2","B44BD2","DC415F","F45C89","20768F",
  "E8B2AC","E49A79","38CADA","34AB37","18AF61","5CF938","6C72E7","741BB2","CC4463","60FEC5","E425E7","BC926B","101C0C","080007","40D32D","C42C03","9027E4","109ADD","581FAA","88C663",
  "001F5B","002436","00254B","0016CB","0017F2","7C6D62","20C9D0","68967B","84FCFE","E48B7F","008865","BC3BAF","3CE072","38484C","A46706","8C5877","7CF05F","804971","6C3E6D","BC6778",
  "D8D1CB","A8FAD8","B817C2","7C11BE","283737","50EAD6","98D6BB","189EFC","ACCF5C","80006E","848E0C","3C15C2","6C709F","C0F2FB","24E314","80E650","90FD61","087045","A88808","A4C361",
  "2CF0EE","5C97F3","D4F46F","6476BA","34E2FD","04489A","F0F61C","8C2937","B09FBA","0C4DE9","E0F5C6","A0EDCD","5CF5DA","18EE69","649ABE","F099BF","94E96A","AC293A","9CFC01","9C35EB",
  "48437C","34A395","787E61","60F81D","38C986","D03311","507A55","209BCD","F0A35A","608373","84AD8D","74428B","2877F1","88A9B7","ECCED7","AC9085","B89047","909C4A","908C43","3C7D0A",
  "14C88B","EC2651","183EEF","607EC9","64D2C4","DC5285","E88152","908158","D0D23C","78E3DE","60BEC4","F8B1DD","F8665A","A8817E","C41411","CCC95D","34318F","1C9180","4CB910","E0925C",
  "1855E3","E450EB","886440","6070C0","F0C371","941625","34A8EB","A483E7","F4AFE7","AC88FD","DC080F","F8E94E","EC2CE2","40BC60","E83617","9C648B","344262","14D00D","645AED","C0B658",
  "48A91C","50BC96","FC2A9C","A056F3","549963","90DD5D","C48466","347C25","CC2DB7","E42B34","3C2EF9","A04EA7","F0989D","B019C6","3866F0","703EAC","9810E8","C0D012","BCA920","48A195",
  "F80377","DCA4CA","8C8FE9","2C200B","8866A5","001CB3","F07960","A0D795","B8FF61","90C1C6","70A2B3","F40F24","4C57CA","48E9F1","1C9E46","C0CCF8","9C4FDA","8489AD","68DBCA","086698",
  "BC5436","044BED","6C8DC1","0CD746","60A37D","9060F1","B8782E","000502","000A95","00264A","041E64","001124","002241","7CC537","78CA39","18E7F4","70CD60","8C7B9D","D89E3F","B8C75D",
  "0C74C2","403004","842999","74E2F5","E0C97A","68A86D","7CC3A1","7073CB","90840D","E80688","EC852F","00F4B9","5C95AE","9803D8","60C547","685B35","2CB43A","689C70","380F4A","3010E4",
  "A886DD","444C0C","B4F0AB","80929F","9C04EB","5C969D","609217","84B153","E06678","48D705","041552","CC785F","88CB87","F0C1F1","843835","8C006D","A8968A","F41BA1","60D9C7","3CAB8E",
  "F82793","907240","908D6C","B8098A","4C7C5F","68644B","C81EE7","A43135","68D93C","00F76F","C88550","7014A6","985AEB","78D75F","E0B52D","6C94F8","C0CECD","C869CD","A4B805","5CADCF",
  "BC6C21","544E90","ACBC32","7CA1AE","3C22FB","940C98","E8FBE9","38EC0D","E490FD","84AB1A","D06544","D8DC40","805FC5","206980","3C4DBE","48262C","147DDA","C4910C","9860CA","4490BB",
  "34FD6A","443583","E06D17","F0B3EC","F465A6","44F21B","74650C","F4D488","682F67","50ED3C","848C8D","0CE441","B82AA9","7864C0","E81CD8","3C0630","74E1B6","F40E01","1495CE","50DE06",
  "CC660A","FC1D43","B8B2F8","98460A","B85D0A","7C9A1D","103025","5462E2","149D99","C09AD0","94B01F","94F6D6","F82D7C","18810E","608C4A","74B587","FCB6D8","241B7A","8CFE57","C0A600",
  "000393","1C1AC0","40831D","DCD3A2","5C1DD9","88AE07","68FEF7","68EF43","D02B20","2C61F6","D4A33D","F0766F","4098AD","6C4D73","7867D7","B8C111","9CE33F","A8BE27","C0A53E","D4619D",
  "14BD61","7C5049","58404E","D0C5F3","BC9FEF","20AB37","60F445","88E87F","9CF48E","5CF7E6","B853AC","203CAE","A03BE3","4C3275","D83062","B88D12","8C8EF2","90B0ED","04D3CF","B48B19",
  "BCEC5D","28A02B","3871DE","7081EB","606944","0452F3","60FB42","64B9E8","001D4F","002312","D8A25E","000A27","183451","0C771A","286ABA","4CB199","C09F42","D023DB","70DEE2","F0CBA1",
  "182032","403CFC","4860BC","3451C9","406C8F","5855CA","DC2B61","40A6D9","60FACD","003EE1","FC253F","04F7E4","34C059","F0D1A9","705681","14109F","040CCE","54EAA8","28E14C","E4C63D",
  "54E43A","04DB56","AC3C0B","701124","042665","EC3586","78FD94","2CBE08","E8802E","006171","8C7C92","B03495","F437B7","AC7F3E","280B5C","ACFDEC","DC9B9C","54724F","D8CF9C","7C6DF8",
  "04E536","A8BBCF","6C4008","40331A","CCC760","BC4CC4","DC3714","20A2E4","28F076","7C0191","80D605","DC2B2A","082CB6","F84E73","F07807","3CCD36","444ADB","309048","D84C90","24D0DF",
  "6C4A85","28F033","BC0963","E87F95","88C08B","4C7CD9","BCA5A9","20E2A8","A0FBC5","007D60","C40B31","149877","88665A","B0E5F9","40E64B","B4FA48","F02F4B","90FFD6","C0280B","9CEA97",
  "C89BAD","14AE857""A06974","EC6488","E42761","98BEDC","0CB789","9C0567","047975","10327E","30FBB8","44D791","8446FE","D82918","D46BA6","CC0577","D462EA","54BAD6","24166D","940B19",
  "70C7F2","88F56E","C8C2FA","CC64A6","F89A78","88F872","EC5623","18022D","48F8DB","64F81C","807D14","20283E","88108F","F4631F","A49B4F","342EB6","AC9232","8C6D77","14579F","B44326",
  "74C14F","B0EB57","F898EF","58F987","A8F5AC","58BAD4","00BE3B","7CA177","242E02","A4933F","F00FEC","AC075F","104400","B0E17E","E4A7C5","F86EEE","786256","B05508","28A6DB","14A0F8",
  "3C678C","407D0F","68CC6E","D8C771","80D4A5","04B0E7","446A2E","5001D9","487B6B","883FD3","18D276","48FD8E","AC6175","244427","5CA86A","042758","9CE374","DCD916","9C741A","E4A8B6",
  "244C07","54511B","D4612E","745AAA","04FE8D","48AD08","4CFB45","009ACD","7CB15D","88CF98","38F889","D07AB5","B43052","80D09B","1C8E5C","904E2B","2008ED","0034FE","C85195","40CBA8",
  "D46E5C","8853D4","04C06F","202BC1","54A51B","002568","781DBA","00259E","D46AA8","B8BC1B","582AF7","4C8BEF","FC48EF","707BE8","4C1FCC","D4B110","E468A3","3400A3","68A828","BC9C31",
  "E4C2D1","845B12","D4F9A1","0022A1","F03F95","185644","9C69D1","D88ADC","10E953","E04007","7817BE","740AE1","B4A898","0CE4A0","BC1AE4","702F35","6C1632","2C1A01","7422BB","B40931",
  "94E7EA","94E4BA","347146","2CC546","0C839A","E0E0FC","F4B78D","A416E7","68E209","A0DE0F","F487C5","105DDC","DC7385","5455D5","3C306F","80E1BF","482CD0","9408C7","C8CA63","9437F7",
  "903FEA","20AB48","CCD73C","24E9CA","FC73FB","241AE6","E4D373","C0BC9A","006151","BC76C5","5CD89E","8454DF","F0F7E7","90808F","D44F67","B4FF98","107100","14AB02","AC471B","008A55",
  "64A28A","24649F","0C1773","E81E92","888E68","78CF2F","A83B5C","F0FEE7","54211D","04495D","484C29","C4D438","C4E287","9C823F","54F607","F0258E","9C746F","3C9BC6","10A4DA","382028",
  "E47727","28D3EA","A8F266","24456B","483871","E8FD35","ECC5D2","A070B7","78B554","689E6A","282B96","F4FBB8","F89753","5894AE","B03ACE","F84CDA","A0D807","2C780E","34B20A","B00875",
  "847637","FC9435","E02481","5CE883","100177","44A191","4459E3","9835ED","084F0A","A8494D","44004D","18CF24","D89B3B","88403B","FC8743","04885F","C850CE","20DA22","20658E","183D5E",
  "88CEFA","70D313","9C1D36","CCBBFE","E43493","342912","14D169","60F18A","10C172","EC8C9A","B48655","D0D783","501D93","3CCD5D","7C7668","C49F4C","0C704A","B4FBF9","506F77","0C41E9",
  "3CE824","940E6B","5C546D","0C8FFF","54B121","101B54","A80C63","5CC307","28B448","BC3F8F","143004","5004B8","44C346","307496","708A09","F02FA7","18DED7","7C11CB","049FCA","50016B",
  "A47174","F4CB52","B808D7","34A2A2","749D8F","F823B2","E0A3AC","BC7574","20A680","8828B3","DCEE06","14D11F","DC094C","1C6758","24BCF8","A4BA76","48DB50","001882","20F3A3","0C37DC",
  "BC7670","24DBAC","1C1D67","2469A5","EC233D","78F5FD","5C7D5E","90671C","BC25E0","F4E3FB","D02DB3","E8CD2D","84A8E4","0C96BF","60E701","E8088B","643E8C","7C6097","CC53B5","60DE44",
  "105172","08E84F","888603","04F938","AC853D","4846FB","E0247F","00464B","80FB06","6889C1","2CCF58","7CA23E","88A2D7","9C37F4","3C4711","A0DF15","18703B","D89E61","347E00","78B46A",
  "6CEBB6","4CF55B","E83F67","3446EC","6C06D6","DC21E2","FC1BD1","582575","28DEE5","44C7FC","7885F4","F4A4D6","08F458","980D51","00ADD5","B8E3B1","285471","B88E82","DC333D","785773",
  "AC6089","843E92","708CB6","50464A","C4A402","183CB7","A4BDC4","5C9157","3066D0","3CB233","C0FFA8","98B3EF","50F958","345840","5C647A","BC7F7B","F0FAC7","DCEF80","A83512","90F644",
  "6C442A","A47CC9","C40D96","A4AC0F","CCFF90","D4D51B","2491BB","645E10","C8E600","309610","C4278C","7C73EB","E48F1D","94B271","A4178B","B098BC","24016F","983F60","C03FDD","9C7370",
  "C4DE7B","6C1A75","6C7637","F85329","8815C5","A4AAFE","F83B7E","7040FF","34D693","C0F6EC","CC208C","389052","C0E018","5CE747","A8FFBA","8C0FC9","304E1B","E8D765","04F03E","78E22C",
  "C0D026","307C4A","D85982","48B25D","C0D46B","9C9567","A47B1A","04BA1C","7C3D2B","FC4DA6","B414E6","AC9929","E8A660","082FE9","984874","1CA681","C8BFFE","082E36","30A2C2","283334",
  "ACB3B5","08318B","3C9D56","70FD45","446747","E419C1","E4FDA1","FCBCD1","7460FA","8CFD18","C40683","94D00D","1C20DB","D0C65B","24DA33","FCAB90","E0CC7A","F47960","4455B1","70192F",
  "DC16B2","24FB65","0CB527","347916","D016B4","D0EFC1","485702","9C713A","2C97B1","F8C39E","1409DC","60DEF3","EC8914","B89436","58D759","804126","ACF970","74D21D","602E20","E472E2",
  "044F4C","1C151F","DCC64B","043389","2C55D3","C81451","F0C850","C80CC8","0425C5","3CFA43","145F94","F09838","F04347","9CB2B2","A8C83A","483C0C","20F17C","346AC2","84AD58","58605F",
  "C4473F","2400BA","24DF6A","7C7D3D","4482E5","E84DD0","0C45BA","B05B67","CCA223","786A89","14B968","5CF96A","50A72B","0CD6BD","00F81C","087A4C","ACE215","346BD3","70723C","ACE87B",
  "F83DFF","285FDB","404D8E","4C5499","F49FF3","240995","84DBAC","94772B","D440F0","04021F","200BC7","F84ABF","78D752","104780","548998","70A8E3","F8E811","94049C","688F84","30D17E",
  "E03676","EC388F","D03E5C","A4DCBE","D494E8","F8BF09","5CB43E","249EAB","AC8D34","246F8C","1C1386","BC2EF6","4455C4","14A32F","04D3B5","00BB1C","E884C6","642CAC","A82BCD","48DC2D",
  "D005E4","30AAE4","10C3AB","2811EC","38EB47","147740","18D98F","189E2C","5471DD","884033","E0D462","909838","0831A4","A8E978","F82F65","942533","F09BB8","8C5EBD","2CA042","ECC01B",
  "9844CE","C083C9","982FF8","40DCA5","30C50F","2868D2","C868DE","D00DF7","4805E2","C4D738","C0F6C2","04C1D8","C8BC9C","B0FEE5","58D061","642315","A4C74B","BC9789","684571","8493A0",
  "9C8E9C","4014AD","E04BA6","C8B6D3","089356","6C146E","44E968","647924","D8CC98","BC9930","6CE874","C469F0","AC3328","E02E3F","BC9A53","DC2727","F042F5","5CBD9A","14A3B4","E86DE9",
  "144658","58BE72","108FFE","D847BB","C833E5","98AD1D","00682B","981A35","60A751","182A57","F83E95","481258","CC3296","449BC1","B02491","98751A","B48901","048C9A","EC3CBB","BCB0E7",
  "5434EF","ACE342","9017C8","E00084","F41D6B","2841C6","B4F58E","243154","2C58E8","F063F9","7CC385","30A1FA","3412F9","BCE265","4CD1A1","88BFE4","340A98","646D6C","C4B8B4","80B575",
  "A4BE2B","A87D12","4CD0CB","505DAC","B4CD27","DC729B","909497","0CC6CC","785860","E8ABF3","6CB749","989C57","E86819","3C7843","5C0979","E4FB5D","8C2505","5425EA","C894BB","10B1F8",
  "38BC01","341E6B","886639","884477","149D09","707990","4CF95D","8421F1","005A13","2C9D1E","68A0F6","446EE5","34B354","50680A","FC3F7C","384C4F","900325","98E7F5","7C1CF1","78F557",
  "E02861","D0D04B","480031","2CAB00","A8CA7B","A08D16","EC4D47","F4559C","80B686","10C61F","CC96A0","F80113","A49947","C8D15E","C07009","8038BC","C4072F","F48E92","00664B","9CC172",
  "247F3C","581F28","ECCB30","F4DCF9","308730","241FA0","18C58A","7054F5","DCD2FC","9017AC","34CDBE","D8490B","9C28EF","F898B9","FCE33C","C4447D","30E98E","ACBD70","C0B5CD","4C5077",
  "30FD65","B0735D","5C78F8","B827C5","78C5F8","B4B055","048C16","B0CCFE","540DF9","006619","447654","7CD9A0","F033E5","B4F18C","E48326","40B6E7","D0B45D","8836CF","A4C54E","D4BBE6",
  "B04502","1C1FF1","14DE39","9016BA","60AAEF","D0F3F5","D44649","9400B0","2848E7","E43EC6","38881E","E4268B","58F2FC","E8136E","4CAE13","DCD444","A446B4","A83759","50F7ED","0C31DC",
  "245F9F","CCB0A8","502873","F82E3F","90A5AF","D8A491","681324","A8C092","7845B3","20DCFD","FC65B3","109D7A","88892F","28E5B0","9CB2E8","14563A","7090B7","3CA37E","48EF61","78F09B",
  "E07726","74E9BF","D8109F","3C5447","006B6F","58AE2B","6CD704","14230A","58AEA8","64D7C0","946010","6413AB","003DE1","7C1B93","DC2D3C","E00CE5","2841EC","7C004D","8C3446","E0E37C",
  "E8EA4D","3CFFD8","4C617E","C0DCD7","3037B3","385247","684AAE","60D755","A8C252","A04147","1CAECB","044A6C","38FB14","F0E4A2","24A52C","7C942A","1CB796","3847BC","549209","745909",
  "68A03E","B8C385","C8A776","A400E2","483FE9","143CC3","A8E544","1C7F2C","88BCC1","8C426D","1C599B","806933","DC9088","902BD2","40EEDD","AC751D","289E97","8C15C7","60FA9D","DC9914",
  "304596","C0F4E6","F4BF80","E0191D","3C15FB","881196","E40EEE","0C2C54","047970","A057E3","BC3D85","2054FA","38378B","002EC7","488EEF","5C0339","84A9C4","A0086F","D06F82","A0F479",
  "844765","C4FF1F","F87588","F44C7F","A0A33B","D0FF98","B0E5ED","C486E9","C0BFC0","A08CF8","14A51A","047503","9C7DA3","A4C64F","8C0D76","84BE52","849FB5","A4CAA0","C88D83","C81FBE",
  "203DB2","48D539","D4A148","D065CA","8CEBC6","B08900","CCCC81","08C021","600810","48435A","C4F081","801382","94FE22","608334","E47E66","94DBDA","9C52F8","BC620E","74A528","E09796",
  "ACCF85","00E0FC","6416F0","88E3AB","C40528","3CDFBD","509F27","80717A","5C4CA9","F4C714","286ED4","001E10","5439DF","283CE4","587F66","64A651","086361","8C34FD","0819A6","3CF808",
  "486276","B41513","AC4E91","283152","74882A","4CB16C","04BD70","E435C8","E8BDD1","5CB395","30F335","74A063","F4E5F2","541310","8CE5EF","54F294","484C86","A01C8D","F4DEAF","60123C",
  "F4A59D","E0F442","F0C42F","C0B47D","C467D1","C8C465","1C4363","8C5AC1","A85AE0","A4B61E","2864B0","04F169","5021EC","8C683A","B46E08","94E9EE","28E34E","F8AF05","90173F","607ECD",
  "0094EC","74452D","80CFA2","F05501","1413FB","B0761B","145120","C0D193","7804E3","A43B0E","2481C7","FC862A","1C3D2F","EC753E","54D9C6","308AF7","D8EF42","80CC12","18AA0F","00E406",
  "44227C","CCB182","C42B44","400634","04E795","C41688","64B0E8","30A998","3898E9","F4C7AA","005043","000CE7","000A00","48B02D","00044B","88124E","00A0C6","649C81","8CFDF0","10DCB69",
  "D0D003","8CB84A","7C8956","88299C","8C79F5","D003DF","FC039F","A4D990","3C576C","08C5E1","E4B021","C048E6","CC6EA4","A816D0","A46CF1","08AED6","9CE063","D03169","149F3C","7C6456",
  "389AF6","E0AA96","507705","C8D7B0","9097F3","7C1C68","C087EB","3096FB","4827EA","7C787E","28395E","8CF5A3","D47AE2","D8E0E1","F83F51","60A4D0","008701","5C9960","101DC0","68EBAE",
  "444E1A","78471D","A07591","0CDFA4","002637","002119","F4D9FB","3C6200","5CA39D","90187C","50CCF8","50B7C3","1C5A3E","A02195","E47CF9","4844F7","001377","002454","E81132","C06599",
  "BC79AD","4C3C16","0073E0","0017D5","001E7D","001DF6","F008F1","58C38B","00E3B2","301966","F0E77E","94350A","001D25","343111","08FD0E","041BBA","889B39","E432CB","BC8CCD","D022BE",
  "EC9BF3","F409D8","10D542","A0821F","F06BCA","AC3613","4849C7","849866","9476B7","4C6641","B8BBAF","60C5AD","CCB11A","E4FAED","288335","DCCF96","A80600","F05A09","503275","08FC88",
  "24F5AA","988389","84A466","C4576E","508569","F8D0BD","78595E","0C1420","94B10A","3CBBFD","A48431","A0B4A5","E4F8EF","E8B4C8","D087E2","F05B7B","B047BF","7C0BC6","205531","5440AD",
  "804E81","FC8F90","9CD35B","A89FBA","30D587","D059E4","14A364","002538","0C2FB0","70CE8C","F4FEFB","B41A1D","30AB6A","60684E","8020FD","B4CE40","04BDBF","BC7ABF","54219D","98B8BC",
  "184E16","C03D03","C418E9","1CE57F","18CE94","783716","00FA21","7C2302","D49DC0","489DD1","B06FE0","60D0A9","007C2D","007204","BCA58B","80CEB9","00C3F4","FC643A","A8515B","88BD45",
  "54FCF0","306A85","4CDD31","D0B128","BC5451","2C4053","ACAFB9","503DA1","3C0518","900628","38AA3C","EC107B","1C232C","0021D1","001FCC","60A10A","8C71F8","CC051B","8C7712","9463D1",
  "0021D2","5C497D","7825AD","ECE09B","78D6F0","BC20A4","08D42B","789ED0","B0C4E7","A00798","001FCD","38ECE4","945103","002490","0023D7","549B12","FCA13E","24C696","94D771","E84E84",
  "001632","E4E0C5","C81479","1CAF05","0016DB","001EE2","20D5BF","5CE8EB","C0BDD1","B479A7","B0DF3A","805719","34BE00","78521A","205EF7","141F78","B47443","F877B8","182195","44783E",
  "14BB6E","28CC01","6CF373","9C3AAF","781FDB","4CA56D","B86CE8","0CB319","183F47","B46293","50A4C8","1867B0","6C8336","ACEE9E","B857D8","AC5A14","08ECA9","7840E4","E09971","10D38A",
  "6077E2","FC1910","3CA10D","646CB2","680571","14B484","000918","00E064","749EF5","68BFC4","04B1A1","CC464E","5050A4","8086D9","386A77","5CC1D7","90EEC7","1029AB","184ECB","A027B6",
  "103917","9880EE","4CC95E","E86DCB","5C10C5","EC7CB6","58A639","F03965","7C0A3F","8CE5C0","F08A76","ECAA25","687D6B","485169","A8DB03","A8346A","3C20F6","7C38AD","F47DEF","7C8BB5",
  "D07FA0","00B5D0","1496E5","8CB0E9","108EE0","FCA621","A8CAB9","641CAE","641CB0","24181D","2802D8","48C796","F4C248","F47190","54B802","448F17","DC44B6","1007B6","342D0D","54BD79",
  "30074D","947BE7","5092B9","DC74A8","C4AE12","D4AE05","F0EE10","20DBAB","E8E5D6","C87E75","00265F","00233A","0007AB","382DD1","FC1F19","840B2D","206432","B407F9","00166C","181EB0",
  "E8039A","30CDA7","001247","001599","0012FB","D0667B","B85E7B","E492FB","6CB7F4","2C4401","B8D9CE","1C66AA","3C8BFE","D4E8B2","1489FD","BC851F","0015B9","002491","002339","5001BB",
  "54880E","F025B7","BC72B1","78F7BE","684898","3423BA","400E85","C8BA94","843838","5C3C27","888322","E89309","78C3E9","8C1ABF","30CBF8","A0CBFD","E45D75","D0FCCC","98398E","C09727",
  "202D07","08373D","C488E5","FCC734","8425DB","B0EC71","E458B8","088C2C","64B853","389496","5056BF","90F1AA","1077B1","A49A58","08EE8B","74458A","38D40B","E83A12","80656D","FCF136",
  "18895B","78BDBC","7CF90E","50F0D3","84119E","244B03","244B81","60AF6D","B85A73","981DFA","C44202","5C2E59","103047","F884F2","E0CBEE","FC0012","0050C29D8","0023C2","000DE5","8031F0",
  "E0D083","D80B9A","88A303","FCDE90","1854CF","807B3E","F8F1E6","D48A39","E4F3C4","08BFA0","701F3C","6C2F8A","1CE61D","E0C377","64037F","B49D02","809FF5","8CDEE6","10EC81","C45D83",
  "102B41","D8A35C","18BFB3","1819D6","20326C","6489F1","E458E7","6CC7EC","C0BDC8","647BCE","A887B3","6C006B","24FCE5","70FD46","8C83E1","889F6F","68E7C2","58B10F","C819F7","3CDCBC",
  "804E70","D4E6B7","5C865C","D80831","982D68","7C2EDD","3CF7A4","E47DBD","842519","24920E","FC4203","A01081","BC4760","04180F","2013E0","002566","DC7144","980C82","A00BBA","606BBD",
  "00214C","0018AF","001EE1","00166B","0000F0","8CC8CD","A8F274","D487D8","184617","380A94","D0DFC7","D0C1B1","8018A7","F47B5E","70F927","C45006","88329B","1449E0","D02544","BC4486",
  "20D390","9401C2","50FC9F","380B40","F0728C","34AA8B","24DBED","84B541","006F64","DC6672","F8E61A","9C2A83","A06090","BC765E","D85B2A","40163B","C83870","647791","9CE6E7","9C0298",
  "28987B","54FA3E","0C8910","78ABBB","D8C4E9","BCD11F","F4428F","446D6C","00F46F","0C715D","2CAE2B","F40E22","C01173","BCE63F","7C9122","2827BF","8CBFA6","C8A823","B0C559","BC1485",
  "083D88","608F5C","109266","745F00","0050C2E6C","DCDCE2","A0AC69","1089FB","A4307A","DC8983","5CCB99","90B144","6CDDBC","2468B0","A830BC","7846D4","BC7E8B","6872C3","70B13D","543AD6",
  "F0F564","6407F6","804786","F40228","0024E9","D85575","D411A3","04BA8D","B8BC5B","90633B","FCAAB6","782327","DCF756","CC2119","702AD5","74EB80","0CE0DC","D868C3","C493D9","A82BB9",
  "705AAC","685ACF","0CA8A7","9C8C6E","00BF61","A407B6","087808","887598","C0174D","04D6AA","08152F","5C5181","608E08","B4BFF6","58C5CB","84C0EF","C0D3C0","948BC1","14568E","682737",
  "2C0E3D","2CBABA","40D3AE","3816D1","D0176A","D48890","5492BE","000278","002399","0017C9","C4731E","5C0A5B","7CF854","001B98","001A8A","3C5A37","F49F54","34C3AC","44F459","00265D",
  "CCF9E8","D857EF","18E2C2","9852B1","E440E2","103B59","D890E8","C462EA","14F42A","0808C2","CCFE3C","28BAB5","182666","30D6C9","CC07AB","002567","BCB1F3","1C62B8","B43A28","78A873",
  "001C43","0023D6","E4121D","EC1F72","E8508B","F8042E","0821EF","34145F","845181","70288B","348A7B","78009E","ACC33A","54F201","1C3ADE","AC5F3E","509EA7","A88195","88ADD2","50F520",
  "64B310","A4EBD3","5CF6DC","B8C68E","04FE31","4CBCA5","D831CF","188331","9C65B0","8455A5","A87C01","B0D09C","50C8E5","48137E","382DE8","C08997","380195","842E27","1432D1","1816C9",
  "E0DB10","9000DB","B4EF39","183A2D","206E9C","6C2F2C","18227E","30C7AE","CC3A61","000DAE","C0DCDA","04B429","48794D","942DDC","245AB5","C0D2DD","AC1E92","3868A4","9CA513","445CE9",
  "140152","98063C","18AB1D","8437D5","3482C5","64E7D8","28C21F","9C5FB0","E87F6B","185BB3","8CEA48","F88F07","A44519","D09C7A","50EC50","70BBE9","04CF8C","40313C","20A60C","8035C1",
  "2047DA","7C49EB","38E60A","4C49E3","00EC0A","3CBD3E","286C07","28E31F","8C53C3","4C6371","D45EEC","88C397","F4308B","58B623","FC1999","241145","4CF202","884604","64644A","50D2F5",
  "28167F","04C807","20F478","E0B655","E0DCFF","EC4118","C82832","48FDA3","A45046","E06267","082525","F460E2","9C2EA1","0CF346","80AD16","64CC2E","2082C0","7C1DD9","A086C6","9C99A0",
  "584498","5CE50C","E85A8B","A44BD5","7CFD6B","B460ED","9C9D7E","188740","341CF0","7CC294","44237C","9078B2","BC7FA4","60AB67","7C035E","1801F1","7C03AB","D832E3","9487E0","50A009",
  "508F4C","7811DC","38A4ED","742344","102AB3","185936","F8A45F","640980","E01F88","6490C1","9C28F7","5448E6","28D127","DCB72E","508E49","047A0B","8CAACE","ECFA5C","C83DDC","2034FB",
  "A89CED","703A51","04E598","482CA0","04D13A","0C9838","04B167","50642B","ACC1EE","B0E235","C46AB7","FC64BA","68DFDD","98FAE3","F0B429","64DDE9","2CD066","EC4D3E","6C0DC4","4C0220",
  "103F44","8CDEF9","9CBCF0","68ABBC","509839","741575","1CCCD6","582059","B4C4FC","7CD661","E446DA","8C5AF8","D8CE3A","E4DB6D","D86375","18F0E4","F4F5DB","ECD09F","34CE00","C40BCB",
  "64B473","7451BA","7802F8","ACF7F3","D4970B","8CBEBE","14F65A","009EC8","0C1DAF","3480B3","F48B32","E0CCF8","98F621","7C2ADB","941700","64A200","503DC6","8CD9D6"};

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
#define I2C_SDA 21
#define I2C_SCL 22

TwoWire I2CBME = TwoWire(0);

// ############################ Debug ############################

// Print on Serial if debug == true
void debug(char * msg )
{
  if (DEBUG)
  {
    Serial.println(msg);
  }
}
// print int values if debug == true
void debug_int(int value)
{
  if (DEBUG)
  {
    Serial.println(value);
  }
}
void debug_hex(int hex)
{
   if (DEBUG)
  {
    Serial.println(hex,HEX);
  }
  
}
// ############################ Interruption ############################

// timer routine
void IRAM_ATTR onTimer() {
  interruptCounter++;
}

// ############################ Fonctions de traduction ############################

//Permet d'obtenir l'adresse MAC d'un WIFI en string 
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

//Conversion d'un string en int
int convStringToInt(char s){
  if(int(s) >= '0' && int(s) <= '9'){
    return int(s) - '0'; 
  }
  else if(int(s) >= 'A' && int(s) <= 'F'){
    return int(s) - 55 ;
  }
  return 0;
}

//Converti un float en hexa
byte convFloatToHex(float x){
  int integer = x * 10;
  Serial.print("Integer : ");
  Serial.println(integer);
  byte res = integer & 0xFF;
  Serial.print("Hexa : ");
  Serial.println(res);
  return res ;
}

//Conversion d'un float en int
int convFloatToInt(float x){
  int integer = x * 100;
  Serial.print("Integer : ");
  Serial.println(integer);
  return integer ;
}

//Converti température en une chaîne de caractère
void convByteToCharTemp(int temp, char *temp_trad){
  String trad = "";
  if (temp < 4096){
      trad.concat('0');
  }
  trad.concat(String(temp,HEX));
  trad.toCharArray(temp_trad, 5); 
  Serial.println(temp_trad);
  
}

//Converti le bssid en une chaîne de caractère
void convByteToCharBssid(byte bssid[6], byte rssi, char *bssid_trad){
  String trad = "";
  for(int i = 0; i < 6; i++){
    if (bssid[i] < 16){
      trad.concat('0');
    }
    trad.concat(String(bssid[i],HEX));
  }
  if (rssi < 16){
      trad.concat(String('0'));
  }
  trad.concat(String(rssi,HEX));
  trad.toCharArray(bssid_trad, 15); 
  Serial.println(bssid_trad);
}

//Convertit des informations de mouvement en une chaîne de caractère
void convByteToCharAcc(int mouv_tab[15], char *x_trad){
  debug("convBytetochar");
  String trad = "";
  for (int i = 0; i < 14; i++)
  {
    trad.concat(String(mouv_tab[i+1],HEX)); // stocker les 14 dernières infos sur le mouvement sur X dans x_trad
  }
  trad.toCharArray(x_trad, 14); 
  debug(x_trad);
  
}

//Moyenne des températures
int moyTemp(int temp_tab[7], int nb){ // NB = 3 ou 4 
  int som = 0;
  if(nb == 3){
    for(int i = 0; i < nb; i++){
    som += temp_tab[i];
    }
    return som/3;
  }
  if(nb == 4){
    for(int i = 3; i < 7; i++){
    som += temp_tab[i];
    }
    return som/4;
  }
  return 0;
}

// ############################ Fonctions pour les capteurs ############################

// Détecte les WIFI au alentour
void detection_wifi(){
  
  Serial.println("scan start");
  String bssid;
  int8_t rssi;
  int nb_add = 0;
  String id_add;
    
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
      
          
   //Construction du message Sigfox pour l'adresse mac la  + puissante
   //Code Sigfox frame (MAC (6B) + RSSI (1B))
    do{
      bssid = WiFi.BSSIDstr(nb_add);
      rssi = WiFi.RSSI(nb_add);
      for (int j = 0; j < 6; j++){
        String xval = getValue(bssid, ':', j);
        byte v1 = convStringToInt(xval[0]) & 0xFF;
        byte v2 = convStringToInt(xval[1]) & 0xFF;
        bssid1[j] = (v1 << 4) + v2 ;
        Serial.print("BSSID : ");
        Serial.println(bssid1[j],HEX);
        if (j < 3){
          if(bssid1[j] < 16){
            id_add.concat('0');
          }
          id_add.concat(String(bssid1[j],HEX));
        }
      }
      Serial.print("Id adresse MAC : ");
      Serial.println(id_add);
      
      Serial.print("Classement du bssid : ");
      Serial.println(nb_add);
      
      nb_add = nb_add + 1;
    }while(!testMacAdresse(id_add));
      
    Serial.print("RSSI:");
    Serial.println(rssi);

    Serial.print("Valeur hexa du rssi : ");
    rssi1 =(rssi >>  0) & 0xFF;
    Serial.println(rssi1, HEX);

    //Construction du message Sigfox pour l'adresse mac la 2eme + puissante 
    //Code Sigfox frame (MAC (6B) + RSSI (2B))
    do{
      bssid = WiFi.BSSIDstr(nb_add);
      rssi = WiFi.RSSI(nb_add);
      id_add = "";
      for (int j = 0; j < 6; j++){
        String xval = getValue(bssid, ':', j);
        byte v1 = convStringToInt(xval[0]) & 0xFF;
        byte v2 = convStringToInt(xval[1]) & 0xFF;
        bssid2[j] = (v1 << 4 ) + v2 ;
        if (j < 3){
          if(bssid2[j] < 16){
            id_add.concat('0');
          }
          id_add.concat(String(bssid2[j],HEX));
        }
        Serial.print("hex version : ");
        Serial.print(bssid2[j],HEX);
      }
      Serial.println("");
      Serial.print("Id adresse MAC : ");
      Serial.println(id_add);
      
      Serial.print("Classement du bssid : ");
      Serial.println(nb_add);
      
      nb_add = nb_add + 1;
    }while(!testMacAdresse(id_add));
    Serial.print("RSSI:");
    Serial.println(rssi);
    
    Serial.print("Valeur hexa du rssi : ");
    rssi2 =(rssi >>  0) & 0xFF;
    Serial.println(rssi2, HEX);
  }
}

void taskOne(void* parameter){
    while(1){
    mma.getEvent(&event);
    //Serial.println("temperature");
    Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
    float tmp = temp.temperature;
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);
    Serial.print("and minutes = ");Serial.println(minutes_acc);
    t = convFloatToInt(tmp);
    temp_tab[totalInterruptCounter] = t;
    if (totalInterruptCounter == 6) {
      for (int i = 0; i < 7; i++) {
        Serial.print("Température n° "); Serial.print(i); Serial.print(": temp : "); Serial.println(temp_tab[i]);
      }
      totalInterruptCounter = 0;
    }
  //accelero 
  //accelerometre
    Serial.println("Accelero mesure");
     mma.getEvent(&event);
      mma.read();
      x_1 = event.acceleration.x;
      y_1 = event.acceleration.y;
      Serial.print("x_1 : ");Serial.println(x_1);
      Serial.print("y_1 : ");Serial.println(y_1);
      diffX = x_1 - x_0;
      diffY = y_1 - y_0;


      if (abs(diffX) > SEUILX)
      {
        countX ++;
        Serial.println("! ! Mouvement sur X detecte ! !");
        mouvX[minutes_acc] = 1; 
      } 
      else
      {
        mouvX[minutes_acc] = 0; 
      }
      if (abs(diffY) > SEUILY)
      {
        countY ++;
        Serial.println("! ! Mouvement sur Y detecte ! !");
        mouvY[minutes_acc] = 1; 
      } 
      else
      {
        mouvY[minutes_acc] = 0; 
      }
      x_0 = x_1;
      y_0 = y_1;

      //remise à zero des compteurs de X et Y
      if (minutes_acc == 15 )
      {
        for (int i = 0; i < 15; i++)
        {
          Serial.print(i);Serial.print("mouv sur X : ");Serial.println(mouvX[i]);
          Serial.print(i);Serial.print("mouv sur Y : ");Serial.println(mouvY[i]);
        }
        
        countX = 0;
        countY = 0;
        minutes_acc = 0 ;
       }
    
    vTaskDelay(DELAY_TASK);
  }
}

// ############################ Tests ############################

// Compare une adresse MAC détectée avec celles présentes dans le tableau mac
bool testMacAdresse(String id_add){
  String id_const;
  for(int i = 0; i < sizeof(mac) / sizeof(mac[0]); i++){ 
      id_const = String(mac[i]);
      if(id_add.equalsIgnoreCase(id_const)){
        Serial.println("Les id sont identiques");
        Serial.print("Id adresse MAC : ");
        Serial.println(id_add);
        Serial.print("Id adresse MAC Constructeur : ");
        Serial.println(id_const);
        return false; 
      }  
   }
   return true; 
}

// Détermination des cas
int testCas(){
  temp1 = moyTemp(temp_tab, 3);
  temp2 = moyTemp(temp_tab, 4);
  if(temp1 < 3798 or temp1 > 3902 or temp2 < 3798 or temp2 > 3902){
    if(Xaddseuil < countX or Yaddseuil < countY){
      if(bssid1 != bssid1_prec and bssid2 != bssid2_prec)
        return 5;
      return 3;
    }
    return 1;
  }
  else{
    if(Xaddseuil < countX or Yaddseuil < countY){
      if(bssid1 != bssid1_prec and bssid2 != bssid2_prec)
        return 4;
      return 2;
    }
    return 0;
  }
}

// ############################ Formation & envoi de messages ############################

//Envoie des messages
void send_message(char* msg){
  //Envoie du message
  Serial2.print(msg);
  Serial.print("Envoi : ");
  Serial.println(msg);
  delay(1000);

  //Attente d'une réponse de sigfox
  while (!Serial2.available()){
    Serial.println("Waiting for response");
    delay(1000);
  }

  //Réponse de sigfox
  while (Serial2.available()){
    output = (char)Serial2.read();
    status += output;
    delay(10);
  }
  Serial.println();
  Serial.print("Status \t");
  Serial.println(status);
  delay(1000);
}


//Formation des messages          A TESTER
void format_message(){
  
  char *mess1 = (char*)malloc(30 * sizeof(char));
  char *mess2 = (char*)malloc(30 * sizeof(char));
  char *bssid1_trad = (char *)malloc(15 * sizeof(char));
  char *bssid2_trad = (char *)malloc(15 * sizeof(char));
  char *temp1_trad = (char *)malloc(5 * sizeof(char));
  char *temp2_trad = (char *)malloc(5 * sizeof(char));
  char *x_trad = (char *)malloc(15 * sizeof(char));
  char *y_trad = (char *)malloc(15 * sizeof(char));

  //Initialisation des messages
  snprintf(mess1, 30 * sizeof(char), "AT$SF=") ;
  snprintf(mess2, 30 * sizeof(char), "AT$SF=") ;

  // Détermination des cas
  int cas = testCas();
  Serial.print("-- on est dans le cas n° ");Serial.println(cas);
  switch(cas){
    
    case 0: // Température normal & aucun mouvement
      break;
      
    case 1: // Température anormal & aucun mouvement
      convByteToCharTemp(temp1, temp1_trad);
      strcat(mess1, "00000000000000");
      strcat(mess1, temp1_trad);
      strcat(mess1, "06");      
      break;
      
    case 2: // Température normal & mouvement présent & pas chgmt localisation

      convByteToCharAcc(mouvX, x_trad);
      convByteToCharAcc(mouvY, y_trad);
       
      strcat(mess1, x_trad);
      strcat(mess2, y_trad);

      strcat(mess1, "00000000000000");
      strcat(mess2, "00000000000000");

      strcat(mess1, "01");
      strcat(mess2, "02");    
      break;
      
    case 3: // Température anormal & mouvement présent & pas chgmt localisation

      convByteToCharAcc(mouvX, x_trad);
      convByteToCharAcc(mouvY, y_trad);
       
      strcat(mess1, x_trad);
      strcat(mess2, y_trad);

//      strcat(mess1, "0000000000");
//      strcat(mess2, "0000000000");
  
      convByteToCharTemp(temp1, temp1_trad);
      convByteToCharTemp(temp2, temp2_trad);
             
      strcat(mess1, temp1_trad);
      strcat(mess2, temp2_trad);

      strcat(mess1, "04");
      strcat(mess2, "05");
      break;
      
    case 4: // Température normal & mouvement présent & chgmt localisation
      
      convByteToCharBssid(bssid1, rssi1, bssid1_trad);
      convByteToCharBssid(bssid2, rssi2, bssid2_trad); 
      
      strcat(mess1, bssid1_trad);
      strcat(mess2, bssid2_trad);

      strcat(mess1, "00");
      strcat(mess2, "00");

      for(int i = 0; i < 6; i++){
        bssid1_prec[i] = bssid1[i];
        bssid2_prec[i] = bssid2[i];
      }
      break;
       
    case 5: // Température anormal & mouvement présent & chgmt localisation

      convByteToCharBssid(bssid1, rssi1, bssid1_trad);
      convByteToCharBssid(bssid2, rssi2, bssid2_trad); 

      strcat(mess1, bssid1_trad);
      strcat(mess2, bssid2_trad);
   
      convByteToCharTemp(temp1, temp1_trad);
      convByteToCharTemp(temp2, temp2_trad);
       
      strcat(mess1, temp1_trad);
      strcat(mess2, temp2_trad);

      strcat(mess1, "03");
      strcat(mess2, "03");
      
      for(int i = 0; i < 6; i++){
        bssid1_prec[i] = bssid1[i];
        bssid2_prec[i] = bssid2[i];
      }
      break;
       
    default:
       break;
  }

  Serial.println(mess1); 
  Serial.println(mess2);

  //Envoie des messages selon le cas
  if(cas == 1)
    send_message(mess1);   
  else if(cas > 1 and cas < 6){
    send_message(mess1); 
    send_message(mess2); 
  }

  //Libération mémoire
  free(mess1);
  free(mess2);
  free(bssid1_trad);
  free(bssid2_trad);
  free(temp1_trad);
  free(temp2_trad);
  free(x_trad);
  free(y_trad);

  
  // Wait a bit before scanning again
  delay(5000);  
}

// ############################ Set up ############################

void setup()
{
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial2.begin(9600);
  Serial.begin(115200);
  pinMode(RX0, INPUT);
  pinMode(TX0, OUTPUT);

  //creation du timer
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, NB_SECOND * 1000000, true);   // 1 000 000 ms = 1 s
  timerAlarmEnable(timer);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  btStop();
  delay(100);

  Serial.println("Setup done");

  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!");

  // Range 2G, 4G, 8G
  mma.setRange(MMA8451_RANGE_2_G);

  // getRange() -> 2G = 1, 4G = 2, 8G = 3
  Serial.print("Range = "); Serial.print(2 << mma.getRange());  
  Serial.println("G");

  Serial.println("SHTC3 test");
  if (!shtc3.begin()) {
    Serial.println("Couldn't find SHTC3");
    while (1) delay(1);
  }
  Serial.println("Found SHTC3 sensor");

  //initialisation des données d'acceleration
  mma.getEvent(&event);
  x_0 = event.acceleration.x;
  y_0 = event.acceleration.y;

  Serial.print("init X0 : ");Serial.println(x_0);
  Serial.print("init Y0 : ");Serial.println(y_0);
  shtc3.getEvent(&humidity, &temp); // populate temp object with fresh data
  
  xTaskCreate(taskOne,"taskOne",100000, NULL, 1,NULL);
}

// ############################ Loop ############################

void loop()
{   
  if (interruptCounter > 0) {
    interruptCounter--;
    minutes_acc++;
    totalInterruptCounter++;
    shtc3.getEvent(&humidity, &temp); // populate temp object with fresh data
  
     if(minutes_acc%15 == 0 and totalInterruptCounter != 0){
        detection_wifi();
        format_message();  
    }
  }
}
