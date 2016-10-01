CppHtmlVirtualMechine是一款基于C++的汇编风格的脚本语言  
#使用方法  
在VS2013中编译，生成CppHtmlCompiler.exe的可执行文件,本    
程序有两种执行方式，命令行模式和脚本模式  
##命令行模式  
可以直接运行CppHtmlCompiler.exe,便可以开始输入命令，如  
echo "Hello World!"  
##脚本模式  
通过命令行传入参数的方式执行cpphtml格式的脚本  
#命令介绍  
halt 停机命令 使用方式:halt  
set 设置变量命令 使用方式:set @var,[@var,"str",num,expr:@a+@b]  
function 函数定义 使用方式：function functionname:  
call 函数调用 使用方式:call functionname  
end 函数结束命令 使用方式:end  
looper 循环标志 使用方式:looper  
back 跳转命令 根据前一个比较的状态跳转到指定位置 使用方式：back mt,eq,lt  
cmp 比较命令 使用方式:cmp [@var,"str",num],[@var,"str",num]  
echo 输出命令 使用方式:echo ["str",@var,"str@{var}"]  
free 释放变量命令 使用方式:free @var  
#cpphtml介绍
cpphtml 语言必须包含在<?cpp和?>中，否则翻译器不会解析  
定义一个变量:cpphtml没有变量类型，所有变量都是全局变量，全局可用  
定义变量方式：  
最简单的形式:set @a,10,定义了变量a，并初始化为10  
变量初始化变量:set @b,@a,定义了变量b，并初始化为变量a的值  
运算表达式：set @a,expr:@a+10,这个表达式既可以初始化变量，又可以对变量  
的值进行修改，如上就是将a的值加10，在set第二个参数中如果是expr:开头，那个这个  
表达式将被视为一个运算表达式。
函数：  
函数使用关键字function定义，以end结束。注意，函数结尾必须是end，否则不会返回，  
还可能造成位置错误。例：  
function example:  
	set @a,100  
	set @b,200  
	set @c,expr:@a+@b  
	end  
函数使用call关键字调用  
call example  