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