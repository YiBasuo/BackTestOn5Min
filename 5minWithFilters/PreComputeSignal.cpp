//------------------------------------------------------------------------
// 简称: PreComputeSignal
// 名称: Pre-Compute Signal on Long Period
// 类别: 公式应用
// 类型: 用户应用
// 输出:
//------------------------------------------------------------------------


Params	
	string signalPeriod("1HrSignal");
	string prev1HrHighTableName("Prev1HrHigh");
	string prev1HrLowTableName("Prev1HrLow");
	
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

Begin
	//*****************************Search for Signal Part**************************//
	signal = SignalSearching();
	
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
// 用户版本	2017/06/03 15:04:52
// 版权所有	qwerrqwer
// 更改声明	TradeBlazer Software保留对TradeBlazer平台
//			每一版本的TradeBlazer公式修改和重写的权利
//------------------------------------------------------------------------