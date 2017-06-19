//------------------------------------------------------------------------
// 简称: testMethod
// 名称: Method for testing
// 类别: 公式应用
// 类型: 用户应用
// 输出:
//------------------------------------------------------------------------
Params	
	string prev1HrHighTableName("Prev1HrHigh");
	string prev1HrLowTableName("Prev1HrLow");
	string signalPeriod("1HrSignal");
	string triggerPriceTableName("TriggerPrice");
	
	Numeric FastLength(12);
	Numeric SlowLength(26);
	Numeric MACDLength(9);
	Numeric TrendLength(60);
	
Vars
	//***********Indicators***********************//
	NumericSeries DIF; 
	Numeric DEA;
	Numeric MACD;
	NumericSeries DIFMain;
	NumericSeries DEAMain;
	NumericSeries MidLine;
	NumericSeries MA60;
	
	//**************Signal*****************//
	Numeric timeModifier(0);
	string strKey;
	
	NumericSeries timestamp;

	// Signal from long period chart
	NumericSeries signalRaw;

	// 5min signal
	Numeric signal;
	Numeric triggerPrice;

	//*******Strategic****************//
	Numeric Lots(1);	
	Numeric prevOneHrHigh;
	Numeric prevOneHrLow;
	
	NumericSeries stopLoss;
	Numeric pressure;
	Numeric support;
	Numeric entryTimestamp;
Begin
	//***************************** Pre-Compute signal on long period *************************//
	// Method Application PreComputeSignal has to be callled upon a #signalPeriod# chart, before this one is loaeded on 5MinSignal
	// The Parameter #signalPeriod# have to be the same as it in PreComputeSignal. 

	//****************** Signal Searching ********************//
	// This function will find all 5Min bars during the signal period and save associated info into a file named "5MinSignal"
	signalRaw = ReadSignalTo5minChart(signalPeriod);
	
	// This function will return all the 5min bars to operate on, and save the trigger price to table
	signal = Find5minSignal(signalRaw);
	
	// Read trigger price from DB
	strKey = DateTimeToString(Date + Time);
	triggerPrice = ReadValueFromDB(triggerPriceTableName, strKey);
	
	//******************* Operate **********************************************************//
	timeModifier = GetTimeModifier(signalPeriod);
	timestamp = Date + TimeModifier;
	
	strKey = DateTimeToString(timestamp);	
	prevOneHrHigh = ReadValueFromDB(prev1HrHighTableName, strKey);
	prevOneHrLow = ReadValueFromDB(prev1HrLowTableName, strKey);

	pressure = GetNearestPeak("pressure");
	support = GetNearestPeak("support");
	
	// No-op at Closing Bar
	if (signal > 0)
	{
		if (time == 0.1455 || Date <> Date[-1])
		{
		}
		else 
		{
			Buy(Lots, triggerPrice);
		}
	}
	
	if (signal < 0)
	{
		if (time == 0.1455 || Date <> Date[-1])
		{
		}
		else 
		{
			SellShort(Lots, triggerPrice);
		}
	}
	
	// Calculate Indicators
	DIF = XAverage( Close, FastLength ) - XAverage( Close, SlowLength ) ;	
	DEA = XAverage(DIF, MACDLength);
	MACD = DIF - DEA;
	
	
	DIFMain = MA60 + DIF * 3.9;
	DEAMain = XAverage(DIFMain, MACDLength);
	MidLine = XAverage(XAverage(C,10),10);
	MA60 = XAverage(Close, 60);
	
	// No Losing Overnight Positions
	if (MarketPosition == 1 && time == 0.1455 && Close <= EntryPrice)
	{
		if (MA60 > MA60[1] && MidLine > MidLine[1] && DEAMain > DEAMain[1] && DEAMain > MidLine && MidLine > MA60)
		{
		}
		else 
		{
			Sell(Lots, Close);
		}
	}
	if (MarketPosition == -1 && time == 0.1455 && Close >= EntryPrice)
	{
		if (MA60 < MA60[1] && MidLine < MidLine[1] && DEAMain < DEAMain[1] && DEAMain < MidLine && MidLine < MA60)
		{
		}
		else 
		{
			BuyToCover(Lots, Close);
		}
	}
	if (MarketPosition == 1 && Date <> Date[-1] && Close <= EntryPrice)
	{
		if (MA60 > MA60[1] && MidLine > MidLine[1] && DEAMain > DEAMain[1] && DEAMain > MidLine && MidLine > MA60)
		{
		}
		else 
		{
			Sell(Lots, Close);
		}
	}
	if (MarketPosition == -1 && Date <> Date[-1] && Close >= EntryPrice)
	{
		if (MA60 < MA60[1] && MidLine < MidLine[1] && DEAMain < DEAMain[1] && DEAMain < MidLine && MidLine < MA60)
		{
		}
		else 
		{
			BuyToCover(Lots, Close);
		}
	}
	
	if (Low < prevOneHrLow And MarketPosition == 1)
	{
		Sell(Lots, prevOneHrLow);
	}
	
	if (High > prevOneHrHigh And MarketPosition == -1)
	{
		BuyToCover(Lots, prevOneHrHigh);
	}

	
/*	if (signal > 0)
	{
		Buy(Lots, triggerPrice);
		
		stopLoss = Max(support, prevOneHrLow);
		entryTimestamp = timestamp;
	}
	
	if (signal < 0)
	{
		SellShort(Lots, triggerPrice);
		
		stopLoss = Min(pressure, prevOneHrHigh);
		entryTimestamp = timestamp;
	}*/
	
	//****************** Holding Strategy *************************************************//	
/*	if (timestamp == entryTimestamp)
	{
		if (signal == 0)
		{
			stopLoss = stopLoss[1];
		}
	}
	else if (timestamp <> timestamp[1] && timestamp == entryTimestamp)
	{
		stopLoss = stopLoss[1];
	}
	else if (timestamp <> timestamp[1] && timestamp <> entryTimestamp)
	{
		if (MarketPosition == 1)
		{
			stopLoss = prevOneHrLow;
		}
		else if (MarketPosition == -1)
		{
			stopLoss = prevOneHrHigh;
		}
		else 
		{
			stopLoss = 0;
		}
	}
	else 
	{
		stopLoss = stopLoss[1];
	}
	
	PlotString("sl", Text(stopLoss), Low - 25);
	
	if (Low < stopLoss And MarketPosition == 1)
	{
		Sell(Lots, stopLoss);
	}
	
	if (High > stopLoss And MarketPosition == -1)
	{
		BuyToCover(Lots, stopLoss);
	}*/
End

//------------------------------------------------------------------------
// 编译版本	GS2015.12.25
// 用户版本	2017/06/05 14:18:25
// 版权所有	qwerrqwer
// 更改声明	TradeBlazer Software保留对TradeBlazer平台
//			每一版本的TradeBlazer公式修改和重写的权利
//------------------------------------------------------------------------