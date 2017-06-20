//------------------------------------------------------------------------
// 简称: PreComputeOneHourWithFilter
// 名称: 
// 类别: 公式应用
// 类型: 用户应用
// 输出:
//------------------------------------------------------------------------

Params	
	string signalPeriod("1HrSignal");
	string prev1HrHighTableName("Prev1HrHigh");
	string prev1HrLowTableName("Prev1HrLow");
	
	Numeric FastLength(12);
	Numeric SlowLength(26);
	Numeric MACDLength(9);
	Numeric TrendLength(60);
	
	//*************Adding 1Hr Filter and Day filter*******************//
	
Vars
	//***********Signal****************//
	// 1 for buy, -1 for sell, and 0 for no-op
	NumericSeries signal;
	
	//******Vars for saving signal data**********//
	// Key is Date + Time; Value is signal
	string strKey;
	string signalValue;
	string triggerPriceValue;
	string abortPriceValue;
	
	//*************Adding 1Hr Filter and Day filter*******************//
	NumericSeries DIF; 
	NumericSeries DEA;
	NumericSeries MACD;
	
	NumericSeries DIFMain;
	NumericSeries DEAMain;
	NumericSeries MidLine;
	NumericSeries MA60;
	
	Bool entry;
	Numeric i;// iterator

	Numeric localHigh;
	Numeric localLow;
	Numeric hoursInOneDirection;
	
	String strKeyDay;
	Numeric prevDate;
	Numeric prevDayHigh;
	Numeric prevDayLow;
	Numeric prevDayOpen;
	Numeric prevDayClose;
	Numeric prevDayDIF;
	Numeric prevDayDEA;
	Numeric prevDayChangeRate;
	Numeric prevDayDIFDirection;
	Numeric prevDayDEADirection;
	Numeric prevDayTrend;
	Numeric daysInOneDirection;
	
Begin
	//*****************************Search for Signal Part**************************//
	signal = SignalSearching();
	
	//***********************************Indicators*****************************//
	// Compute MACD Value:
	DIF = XAverage( Close, FastLength ) - XAverage( Close, SlowLength ) ;	
	DEA = XAverage(DIF, MACDLength);
	MACD = DIF - DEA;
	
	
	DIFMain = MA60 + DIF * 3.9;
	DEAMain = XAverage(DIFMain, MACDLength);
	MidLine = XAverage(XAverage(C,10),10);
	MA60 = XAverage(Close, 60);
	
	PlotNumeric("Dif", DIFMain);
	PlotNumeric("DEA", DEAMain);

	entry = True;
	
	//***********************Day Chart Filter*********************************//
	// Read Day Chart Data
	prevDate = GetPrevdate();
	strKeyDay = DateTimeToString(prevDate);

	prevDayHigh = ReadValueFromDB("DayHigh", strKeyDay);
	prevDayLow = ReadValueFromDB("DayLow", strKeyDay);
	prevDayOpen = ReadValueFromDB("DayOpen", strKeyDay);
	prevDayClose = ReadValueFromDB("DayClose", strKeyDay);
	prevDayDIF = ReadValueFromDB("DayDIFMain", strKeyDay);
	prevDayDEA = ReadValueFromDB("DayDEAMain", strKeyDay);

	prevDayChangeRate = ReadValueFromDB("DayChangeRate", strKeyDay);
	prevDayDIFDirection = ReadValueFromDB("DayDIFDirection", strKeyDay);
	prevDayDEADirection = ReadValueFromDB("DayDEADirection", strKeyDay);
	prevDayTrend = ReadValueFromDB("DayTrend", strKeyDay);
	daysInOneDirection = ReadValueFromDB("DayDaysInOneDirection", strKeyDay);
	
	//***********************************One Hour Signal Filters Here*****************************//
	
	//***********************For trend signal*********************************//
	if (MA60 > MA60[1] && MidLine > MidLine[1] && DEAMain > MidLine && MidLine > MA60 && signal > 0 && entry)
	{
		// Filter out: Signal price is new highest
		localHigh = High[1];
		for i = 1 to 100
		{
			if (DEAMain[i] <= DEAMain[i + 1])
			{
				break;
			}
			localHigh = Max(High[i], localHigh);
		}
		if (High >= localHigh)
		{
			entry = False;
		}
	}
	else if (MA60 < MA60[1] && MidLine < MidLine[1] && DEAMain < MidLine && MidLine < MA60 && signal < 0 && entry)
	{
		// Filter out: signal price is new lowest
		localLow = Low[1];
		for i = 1 to 100
		{
			if (DEAMain[i] >= DEAMain[i + 1])
			{
				break;
			}
			localLow = Min(Low[i], localLow);
		}
		if (Low <= localLow)
		{
			entry = False;
		}
	}
	
	//*************************For anti-trend signals**********************//
	//rule out those go against 1 Hr DEA. This will decrease the profit, but increase accuracy and effeciency greatly
	if (MA60 > MA60[1] && MidLine > MidLine[1] && DEAMain > MidLine && MidLine > MA60 && signal < 0 && entry)
	{
		if (DEAMain < DEAMain[1])
		{
			entry = True;
		}
		else
		{
			entry = False;
		}
	}
	else if (MA60 < MA60[1] && MidLine < MidLine[1] && DEAMain < MidLine && MidLine < MA60 && signal > 0 && entry)
	{
		if (DEAMain > DEAMain[1])
		{
			entry = True;
		}
		else
		{
			entry = False;
		}
	}
	
	//******************************For non-trend signals*********************************//
 	if (not (MA60 > MA60[1] && MidLine > MidLine[1] && DEAMain > MidLine && MidLine > MA60) && not (MA60 < MA60[1] && MidLine < MidLine[1] && DEAMain < MidLine && MidLine < MA60))
	{
		if (MA60 > MA60[1] && MidLine > MidLine[1])// && DEAMain > DEAMain[1])
		{
			if (signal > 0)
			{
				entry = True;
			}
			else
			{
				entry = False;
			}
		}
		else if (MA60 < MA60[1] && MidLine < MidLine[1])// && DEAMain < DEAMain[1])
		{
			if (signal < 0)
			{
				entry = True;
			}
			else
			{
				entry = False;
			}
		}
		else
		{
			entry = False;
		}
	}
	
	if (entry)
	{
		signal = signal;
	}
	else
	{
		signal = 0;
	}

	//***************************Save Signal to a File Part********************************//

	strKey = DateTimeToString(Date + Time);
	signalValue = Text(signal);
	
	//*************Save all signals into DB***************************//
	SetTBProfileString(signalPeriod, strKey, signalValue);
	
	SetTBProfileString(prev1HrHighTableName, strKey, Text(High[1]));
	SetTBProfileString(prev1HrLowTableName, strKey, Text(Low[1]));
	
	SetTBProfileString("1HrHigh", strKey, Text(High));
	SetTBProfileString("1HrLow", strKey, Text(Low));
End

//------------------------------------------------------------------------
// 编译版本	GS2015.12.25
// 用户版本	2017/06/16 16:48:32
// 版权所有	qwerrqwer
// 更改声明	TradeBlazer Software保留对TradeBlazer平台
//			每一版本的TradeBlazer公式修改和重写的权利
//------------------------------------------------------------------------