//------------------------------------------------------------------------
// 简称: GetTimeModifier
// 名称: Get TimeModifier from signalPeriod
// 类别: 用户函数
// 类型: 用户函数
// 输出: 数值型
//------------------------------------------------------------------------

Params
	string signalPeriod;
Vars
	Numeric timeModifier(0);
Begin
	if (signalPeriod == "1HrSignal")
	{
		timeModifier = (Time * 10000 - (Time *10000) % 100) / 10000;
	}
	
	if (signalPeriod == "DaySignal")
	{
		timeModifier = 0;
	}
	
	return timeModifier;
End

//------------------------------------------------------------------------
// 编译版本	GS2015.12.25
// 用户版本	2017/06/03 19:53:34
// 版权所有	qwerrqwer
// 更改声明	TradeBlazer Software保留对TradeBlazer平台
//			每一版本的TradeBlazer公式修改和重写的权利
//------------------------------------------------------------------------