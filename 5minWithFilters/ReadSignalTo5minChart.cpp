//------------------------------------------------------------------------
// 简称: ConvertRawSignalInfoTo5Min
// 名称: Convert raw signal information to 5min signal information
// 类别: 用户函数
// 类型: 用户函数
// 输出: 数值型
//------------------------------------------------------------------------

Params
	string signalPeriod("1HrSignal");
Vars
	NumericSeries signalOn5min;
	
	//***** Time Converter *****//
	Numeric timeModifier(0);
	
	// Vars to read from file
	string strKey;
Begin
	timeModifier = GetTimeModifier(signalPeriod);
	
	strKey = DateTimeToString(Date + TimeModifier);
	
	//***********************Read from file***********************//
	signalOn5min = ReadValueFromDB(signalPeriod, strKey);
	
	return signalOn5min;
End

//------------------------------------------------------------------------
// 编译版本	GS2015.12.25
// 用户版本	2017/06/03 15:28:00
// 版权所有	qwerrqwer
// 更改声明	TradeBlazer Software保留对TradeBlazer平台
//			每一版本的TradeBlazer公式修改和重写的权利
//------------------------------------------------------------------------