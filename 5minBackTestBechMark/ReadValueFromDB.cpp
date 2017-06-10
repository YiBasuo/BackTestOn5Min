//------------------------------------------------------------------------
// 简称: ReadMultiPeriodDataFromFile
// 名称: Read data of another period from a file
// 类别: 用户函数
// 类型: 用户函数
// 输出: 数值型
//------------------------------------------------------------------------

Params
	string FileName("");
	string strKey("");
Vars
	string strValue;
	NumericSeries NumValue;
Begin
	strValue = GetTBProfileString(FileName, strKey);
	if (strValue != InvalidString)
	{
		NumValue = Value(strValue);
	}
	else
	{
		NumValue = NumValue[1];
	}

	Return NumValue;
End

//------------------------------------------------------------------------
// 编译版本	GS2015.12.25
// 用户版本	2017/06/01 02:53:31
// 版权所有	qwerrqwer
// 更改声明	TradeBlazer Software保留对TradeBlazer平台
//			每一版本的TradeBlazer公式修改和重写的权利
//------------------------------------------------------------------------