CppHtmlVirtualMechine��һ�����C++�Ļ����Ľű�����  
#ʹ�÷���  
��VS2013�б��룬����CppHtmlCompiler.exe�Ŀ�ִ���ļ�,��    
����������ִ�з�ʽ��������ģʽ�ͽű�ģʽ  
##������ģʽ  
����ֱ������CppHtmlCompiler.exe,����Կ�ʼ���������  
		echo "Hello World!"  
##�ű�ģʽ  
ͨ�������д�������ķ�ʽִ��cpphtml��ʽ�Ľű�  
#�������  
		halt ͣ������ ʹ�÷�ʽ:halt  
		set ���ñ������� ʹ�÷�ʽ:set @var,[@var,"str",num,expr:@a+@b]  
		function �������� ʹ�÷�ʽ��function functionname:  
		call �������� ʹ�÷�ʽ:call functionname  
		end ������������ ʹ�÷�ʽ:end  
		looper ѭ����־ ʹ�÷�ʽ:looper  
		back ��ת���� ����ǰһ���Ƚϵ�״̬��ת��ָ��λ�� ʹ�÷�ʽ��back mt,eq,lt  
		cmp �Ƚ����� ʹ�÷�ʽ:cmp [@var,"str",num],[@var,"str",num]  
		echo ������� ʹ�÷�ʽ:echo ["str",@var,"str@{var}"]  
		free �ͷű������� ʹ�÷�ʽ:free @var  
#cpphtml����
cpphtml ���Ա��������<?cpp��?>�У����������������  
����һ������:cpphtmlû�б������ͣ����б�������ȫ�ֱ�����ȫ�ֿ���  
##���������ʽ��  
��򵥵���ʽ:set @a,10,�����˱���a������ʼ��Ϊ10  
������ʼ������:set @b,@a,�����˱���b������ʼ��Ϊ����a��ֵ  
������ʽ��set @a,expr:@a+10,������ʽ�ȿ��Գ�ʼ���������ֿ��ԶԱ���  
��ֵ�����޸ģ����Ͼ��ǽ�a��ֵ��10����set�ڶ��������������expr:��ͷ���Ǹ����  
���ʽ������Ϊһ��������ʽ��
##������  
����ʹ�ùؼ���function���壬��end������ע�⣬������β������end�����򲻻᷵�أ�  
���������λ�ô�������
### ��������  
		function example:  
			set @a,100  
			set @b,200  
			set @c,expr:@a+@b  
			end  
����ʹ��call�ؼ��ֵ���  
### ��������
		call example  
## ѭ��:  
ѭ��ʹ�ùؼ���looper����,���cmp��back����ʵ�����̿���,���磺  
### ѭ��  
		set @i,0
		looper
			echo "loop:@{i}"
			set @i,expr:@i+1
			cmp @i,10
			back next,next,looper
��������ʵ�������0��9��Ŀ��  
### ѭ��Ƕ��
		set @j,0
		looper
			echo "out loop:@{j}"
			set @i,0
			looper
				echo "inner loop:@{i}"
				set @i,expr:@i+1
				cmp @i,3
				back next,next,looper
			set @j,expr:@j+1
			back next,next,looper
