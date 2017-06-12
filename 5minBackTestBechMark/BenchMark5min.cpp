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
	
Vars
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
	if (signal > 0)
	{
		if (Open > triggerPrice)
		{
			Buy(Lots, Open);
		}
		else
		{
			Buy(Lots, triggerPrice);
		}
	}
	
	if (signal < 0)
	{
		if (Open < triggerPrice)
		{
			SellShort(Lots, Open);
		}
		else
		{
			SellShort(Lots, triggerPrice);
		}
	}
	
	//****************** Holding Strategy *************************************************//
	timeModifier = GetTimeModifier(signalPeriod);
	timestamp = Date + TimeModifier;

	strKey = DateTimeToString(timestamp);	
	prevOneHrHigh = ReadValueFromDB(prev1HrHighTableName, strKey);
	prevOneHrLow = ReadValueFromDB(prev1HrLowTableName, strKey);
	
	if (Low < prevOneHrLow And MarketPosition == 1)
	{
		Sell(Lots, prevOneHrLow);
	}
	
	if (High > prevOneHrHigh And MarketPosition == -1)
	{
		BuyToCover(Lots, prevOneHrHigh);
	}

End

//------------------------------------------------------------------------
// 编译版本	GS2015.12.25
// 用户版本	2017/06/05 14:18:25
// 版权所有	qwerrqwer
// 更改声明	TradeBlazer Software保留对TradeBlazer平台
//			每一版本的TradeBlazer公式修改和重写的权利
//------------------------------------------------------------------------