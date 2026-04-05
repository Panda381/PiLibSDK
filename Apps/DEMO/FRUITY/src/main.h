
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

#define LOGO_W	350
#define LOGO_H	360
extern const u8 LogoImgJpg[46105];

/*
// sound format: MP3, MPEG-1 layer 3, 80 kbps, 32 kHz
extern const u8 Loop001Snd[88920];
extern const u8 Loop002Snd[54000];
extern const u8 Loop003Snd[34920];
extern const u8 Loop004Snd[69120];
extern const u8 Loop005Snd[27720];
extern const u8 Loop006Snd[70920];
extern const u8 Loop007Snd[39240];
extern const u8 Loop008Snd[40680];
extern const u8 Loop009Snd[72000];
extern const u8 Loop010Snd[34920];
extern const u8 Loop011Snd[44280];
extern const u8 Loop012Snd[126000];
extern const u8 Loop013Snd[66960];
extern const u8 Loop014Snd[24840];
extern const u8 Loop015Snd[69120];
extern const u8 Loop016Snd[160560];
extern const u8 Loop017Snd[38880];
extern const u8 Loop018Snd[40680];
extern const u8 Loop019Snd[37800];
extern const u8 Loop020Snd[36000];
extern const u8 Loop021Snd[72360];
extern const u8 Loop022Snd[51120];
extern const u8 Loop023Snd[75600];
extern const u8 Loop024Snd[28800];
extern const u8 Loop025Snd[77040];
extern const u8 Loop026Snd[49680];
extern const u8 Loop027Snd[69120];
extern const u8 Loop028Snd[34920];
extern const u8 Loop029Snd[69120];
extern const u8 Loop030Snd[36720];
extern const u8 Loop031Snd[19800];
extern const u8 Loop032Snd[59400];
extern const u8 Loop033Snd[51840];
extern const u8 Loop034Snd[80640];
extern const u8 Loop035Snd[65880];
extern const u8 Loop036Snd[140760];
extern const u8 Loop037Snd[46440];
extern const u8 Loop038Snd[99720];
extern const u8 Loop039Snd[46800];
extern const u8 Loop040Snd[74520];
extern const u8 Loop041Snd[110520];
extern const u8 Loop042Snd[137520];
extern const u8 Loop043Snd[69120];
extern const u8 Loop044Snd[69120];
extern const u8 Loop045Snd[52920];
extern const u8 Loop046Snd[69120];
extern const u8 Loop047Snd[82800];
extern const u8 Loop048Snd[80640];
extern const u8 Loop049Snd[113760];
extern const u8 Loop050Snd[96840];
extern const u8 Loop051Snd[87480];
extern const u8 Loop052Snd[30240];
extern const u8 Loop053Snd[153000];
extern const u8 Loop054Snd[22320];
extern const u8 Loop055Snd[18000];
extern const u8 Loop056Snd[22680];
extern const u8 Loop057Snd[104040];
extern const u8 Loop058Snd[104040];
extern const u8 Loop059Snd[19080];
extern const u8 Loop060Snd[11880];
extern const u8 Loop061Snd[69120];
extern const u8 Loop062Snd[69120];
extern const u8 Loop063Snd[141840];
extern const u8 Loop064Snd[61920];
extern const u8 Loop065Snd[34920];
extern const u8 Loop066Snd[69120];
extern const u8 Loop067Snd[137880];
extern const u8 Loop068Snd[38520];
extern const u8 Loop069Snd[160560];
extern const u8 Loop070Snd[128880];
extern const u8 Loop071Snd[83520];
extern const u8 Loop072Snd[71280];
extern const u8 Loop073Snd[122400];
extern const u8 Loop074Snd[96840];
extern const u8 Loop075Snd[74520];
extern const u8 Loop076Snd[87840];
extern const u8 Loop077Snd[87840];
extern const u8 Loop078Snd[107280];
extern const u8 Loop079Snd[36000];
extern const u8 Loop080Snd[126360];
extern const u8 Loop081Snd[160560];
extern const u8 Loop082Snd[87840];
extern const u8 Loop083Snd[160560];
extern const u8 Loop084Snd[73440];
extern const u8 Loop085Snd[75600];
extern const u8 Loop086Snd[72360];
extern const u8 Loop087Snd[32760];
extern const u8 Loop088Snd[106200];
extern const u8 Loop089Snd[136080];
extern const u8 Loop090Snd[92160];
extern const u8 Loop091Snd[122400];
extern const u8 Loop092Snd[156960];
extern const u8 Loop093Snd[69120];
extern const u8 Loop094Snd[69120];
extern const u8 Loop095Snd[69120];
extern const u8 Loop096Snd[20880];
extern const u8 Loop097Snd[80640];
extern const u8 Loop098Snd[104040];
extern const u8 Loop099Snd[98280];
extern const u8 Loop100Snd[97920];
extern const u8 Loop101Snd[128880];
extern const u8 Loop102Snd[66960];
extern const u8 Loop103Snd[64800];
extern const u8 Loop104Snd[64800];
extern const u8 Loop105Snd[131400];
extern const u8 Loop106Snd[128520];
extern const u8 Loop107Snd[9000];
extern const u8 Loop108Snd[70200];
extern const u8 Loop109Snd[19080];
extern const u8 Loop110Snd[128880];
extern const u8 Loop111Snd[55800];
extern const u8 Loop112Snd[73440];
extern const u8 Loop113Snd[40680];
extern const u8 Loop114Snd[20520];
extern const u8 Loop115Snd[9360];
extern const u8 Loop116Snd[66240];
extern const u8 Loop117Snd[19440];
extern const u8 Loop118Snd[71640];
extern const u8 Loop119Snd[35280];
extern const u8 Loop120Snd[136800];
extern const u8 Loop121Snd[68760];
extern const u8 Loop122Snd[123120];
extern const u8 Loop123Snd[90360];
extern const u8 Loop124Snd[65880];
extern const u8 Loop125Snd[137520];
extern const u8 Loop126Snd[140040];
extern const u8 Loop127Snd[35280];
extern const u8 Loop128Snd[69480];
extern const u8 Loop129Snd[16920];
extern const u8 Loop130Snd[24120];
*/

#define LOOP_NUM	130	// number of music loops
#define LOOP_FIRST	1	// index of first music loop

#endif // _MAIN_H
