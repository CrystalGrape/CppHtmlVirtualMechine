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
		###1.0�¼�������
		#import ģ�鵼��  
		sysvar ������ͼ����  
		setview ѡ����ͼ  
		readview ��ȡ��ͼ  
		if ��������  
		else ����ת��  
		fi �������ƿ����  
		for ѭ��  
		rof ѭ������  
		break ����ѭ��  
		continue ����ѭ��  
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
# �汾1.0  
###�汾˵��  
����һϵ�������ԣ�����ģ�顢��ͼ������forѭ����if�����ж�  
### ����ģ��  
������ģ���������ԣ����ùؼ���#import������Ҫ���õ�ģ�飬ģ�鱣����һ��׺��  
Ϊchmodule���ļ��У�ģ�����������Ҫ������<?cpp��?>���С�  
����һ��testģ�飬����Ϊtest.chmodule  
		function testfunc:  
		echo "this is test"  
		set @return,expr:@arg1+@arg2  
		end  
����ʱʹ��  
		#import test  
�����������ش��������������δ�����Զ��ڵ�ǰĿ¼���ҵ�test.chmodule�ļ��������ؽ�  
��ǰ�ļ��С�
### ��ͼ����  
��ͼ������һ��json���飬һ�����ڿ���������ͼ����ģ�Ͳ������������������sysvar��setview��  
readview��  
sysvar������������ֵһ����ͼ����  
setview����ѡ�������Ҫʹ�õ���ͼ����  
readview���ڶ�ȡ��ͼ�е����ݵ�һ��������Ž�����ڶ��������������±꣬��������������ָ��  
��Ҫ��ȡ���ֶ���  
����  
		sysvar @a,[{"name":"aaaa","age":21},{"name":"bbb","age":22}]  
		setview @a  
		readview @name,@index,"name"  
### if��������  
���ӣ�  
		if 10>1  
			echo "1"  
			if 20<10  
				echo "2"  
			else  
				echo "3"  
			fi  
		else  
			echo "4"  
		fi  
����������һ��ifǶ��
### forѭ������
Ŀǰforѭ����֧�ָ����жϣ�forѭ��������������  
����һ����������  
���������˳�����  
������������  
���ӣ�
		for @i,10,1
			echo @i
			if @i>5
				continue
			else
				break
			fi
		rof
�������룬���жϱ���i�Ƿ����10�������˳�ѭ���������ڽ���ѭ����  
ÿ��ѭ������i��һ  