
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// increase time entry
void TimeInc(int cur)
{
	int n;

	// load current time
	sDateTime dt;
	DateTimeUnpack(&dt, UnixTime, 0, 0);

	switch(cur)
	{
	// year
	case 0:	dt.year++;
		if (dt.year >= 2100) dt.year = 1970;
		n = YearIsLeap(dt.year) ? DaysInMonthLeap[dt.mon - 1] : DaysInMonth[dt.mon - 1];
		if (dt.day > n) dt.day = n;
		break;

	// month
	case 1:	dt.mon++;
		if (dt.mon > 12) dt.mon = 1;
		n = YearIsLeap(dt.year) ? DaysInMonthLeap[dt.mon - 1] : DaysInMonth[dt.mon - 1];
		if (dt.day > n) dt.day = n;
		break;

	// day
	case 2:	n = YearIsLeap(dt.year) ? DaysInMonthLeap[dt.mon - 1] : DaysInMonth[dt.mon - 1];
		dt.day++;
		if (dt.day > n) dt.day = 1;
		break;

	// hour
	case 3:	dt.hour++;
		if (dt.hour > 23) dt.hour = 0;
		break;

	// minutes
	case 4:	dt.min++;
		if (dt.min > 59) dt.min = 0;
		break;

	// seconds
	case 5:	dt.sec++;
		if (dt.sec > 59) dt.sec = 0;
		break;
	}

	// set new time
	UnixTime = DateTimePack(&dt);
	if (RTC.ok)
	{
		DateTimeUnpack(&dt, UnixTime, 0, 0);
		if (RTC.Write(&dt))
		{
			if (RTC.Read(&dt))
			{
				UnixTime = DateTimePack(&dt);
			}
			else
				RTC.ok = False;
		}
		else
			RTC.ok = False;
	}
}

// decrease time entry
void TimeDec(int cur)
{
	int n;

	// load current time
	sDateTime dt;
	DateTimeUnpack(&dt, UnixTime, 0, 0);

	switch(cur)
	{
	// year
	case 0:	dt.year--;
		if (dt.year < 1970) dt.year = 2099;
		n = YearIsLeap(dt.year) ? DaysInMonthLeap[dt.mon - 1] : DaysInMonth[dt.mon - 1];
		if (dt.day > n) dt.day = n;
		break;

	// month
	case 1:	dt.mon--;
		if (dt.mon < 1) dt.mon = 12;
		n = YearIsLeap(dt.year) ? DaysInMonthLeap[dt.mon - 1] : DaysInMonth[dt.mon - 1];
		if (dt.day > n) dt.day = n;
		break;

	// day
	case 2:	n = YearIsLeap(dt.year) ? DaysInMonthLeap[dt.mon - 1] : DaysInMonth[dt.mon - 1];
		dt.day--;
		if (dt.day < 1) dt.day = n;
		break;

	// hour
	case 3:	dt.hour--;
		if (dt.hour < 0) dt.hour = 23;
		break;

	// minutes
	case 4:	dt.min--;
		if (dt.min < 0) dt.min = 59;
		break;

	// seconds
	case 5:	dt.sec--;
		if (dt.sec < 0) dt.sec = 59;
		break;
	}

	// set new time
	UnixTime = DateTimePack(&dt);
	if (RTC.ok)
	{
		DateTimeUnpack(&dt, UnixTime, 0, 0);
		if (RTC.Write(&dt))
		{
			if (RTC.Read(&dt))
			{
				UnixTime = DateTimePack(&dt);
			}
			else
				RTC.ok = False;
		}
		else
			RTC.ok = False;
	}
}

// main function
int main()
{
	int key;
	string txt;
	u32 ut;
	sDateTime dt;

	int cur = 0; // current cursor

	// update RTC
	RTC.Init();
	if (RTC.ok)
	{
		if (RTC.Read(&dt))
		{
			UnixTime = DateTimePack(&dt);
		}
		else
			RTC.ok = False;
	}

	// wait
	while (True)
	{
		// update RTC
		Bool oldok = RTC.ok;
		RTC.Init();
		if (RTC.ok && !oldok)
		{
			if (RTC.Read(&dt))
			{
				UnixTime = DateTimePack(&dt);
			}
			else
				RTC.ok = False;
		}

		// RTC info
		DrawText("RTC DS3231:", 0, HEIGHT-16, COL_GRAY);
		DrawText(RTC.ok ? "OK" : "not installed", 12*8, HEIGHT-16, COL_GRAY);

		// get Unix time
		ut = UnixTime;

		// display date "Friday, June 12, 2026"
		txt.SetDow(ut, DOWFORM_LONG);
		txt += ", ";
		txt.AddDateUnix(ut, DATEFORM_USA_NdY);
		DrawTextBg(txt.str(), 8, 0, COL_GREEN, COL_BLACK);

		// display time "22:21:57"
		txt.SetTime(ut, HOURFORM_ZERO, SECFORM_SEC, TIMESEP_DOT);
		DrawTextBg(txt.str(), WIDTH-8*8-8, 0, COL_AZURE, COL_BLACK);

#define YY 110
		// title
		DrawTextBg("Y Y Y Y   M M   D D   h h   m m   s s", 8+4, YY - 14, COL_GRAY, COL_BLACK);

		// edit date and time YYYY-MM-DD HH:MM:SS
		txt.SetDateUnix(ut, DATEFORM_ISO_YMD);
		txt += ' ';
		txt.AddTime(ut, HOURFORM_ZERO, SECFORM_SEC, TIMESEP_DOT, 0);
		DrawTextBgScale(txt.str(), 8, YY, 2, 3, COL_YELLOW, COL_BLACK);

		// draw cursor
		switch(cur)
		{
		// year
		case 0:	DrawTextBgLenScale(txt.str(), 4, 8, YY, 2, 3, COL_BLACK, COL_YELLOW);
			break;

		// month
		case 1:	DrawTextBgLenScale(txt.str()+5, 2, 8+5*2*8, YY, 2, 3, COL_BLACK, COL_YELLOW);
			break;

		// day
		case 2:	DrawTextBgLenScale(txt.str()+8, 2, 8+8*2*8, YY, 2, 3, COL_BLACK, COL_YELLOW);
			break;

		// hour
		case 3:	DrawTextBgLenScale(txt.str()+11, 2, 8+11*2*8, YY, 2, 3, COL_BLACK, COL_YELLOW);
			break;

		// minutes
		case 4:	DrawTextBgLenScale(txt.str()+14, 2, 8+14*2*8, YY, 2, 3, COL_BLACK, COL_YELLOW);
			break;

		// seconds
		case 5:	DrawTextBgLenScale(txt.str()+17, 2, 8+17*2*8, YY, 2, 3, COL_BLACK, COL_YELLOW);
			break;
		}

		// display update
		DispUpdate();

		// clear
		DrawClear();

		// keyboard
		key = KeyGet();
		switch (key)
		{
		// Left
		case KEY_LEFT:
			cur--;
			if (cur < 0) cur = 5;
			break;

		// Right
		case KEY_RIGHT:
			cur++;
			if (cur > 5) cur = 0;
			break;

		// Up
		case KEY_UP:
			TimeInc(cur);
			break;

		// Down
		case KEY_DOWN:
			TimeDec(cur);
			break;

		// Y: Program exit
		case KEY_PAD_Y:
			Reboot();
			break;

		// Alt+X: Screenshot (This may take a few seconds to write.)
		case KEY_SCREENSHOT:
			ScreenShot();
			break;
		}
	}
}
