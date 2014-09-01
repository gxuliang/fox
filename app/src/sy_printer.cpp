
#include "base/sy_types.h"
#include "NetService/sy_netService.h"

#include "sy_printer.h"
#include "base/sy_types.h"
#include "base/sy_debug.h"


const char PP[] = "/dev/pp";
const char Welcome[] = "/******************************/\r\n/*****欢迎使用打印服务系统%(^&*())*****/\r\n";
IPrinter::IPrinter(int type, CConfigTable& tb,IPrinter* pPrinter):m_mutex(CMutex::mutexRecursive)
{
	this->rd=0;
	this->wr=0;
	this->fullflag=false;
	this->loopflag= true;
	this->mtype = type;
	//std::cout <<  tb << std::endl;
	infof("type = %d\n", type);
	//if(this->mtype == 1)//测试的是输出打印机
	{
		this->fd = open(PP, O_RDWR);
		if(this->fd <= 0)
		{
			errorf("open pp failed\n");
		}
		infof("fd = %d\n", this->fd);
	}
	if(this->mtype == 1)//测试的是输出打印机
	{
		//if(tb["welcomeauto"].asString() == "wyes")
		//	this->put(Welcome, sizeof(Welcome));

		//int test_fd = open("test.txt", O_RDWR);
		//char test_buf[2048]="";
		//int test_len = read(test_fd, test_buf, 2048);
		//debugf("===fd = %d, len = %d====\n", test_fd, test_len);
		//this->put(test_buf, test_len);
		//close(test_fd);
		
		#if 0
		int len;
		for(int i = 0;i<3;i++){
		len = write(this->fd, (const char*)test_buf, test_len);
		if(len != test_len)
		{
			errorf("write failed!len = %d, fd = %d\n", len, this->fd);
		}
		}
		#endif
		
	}
	else
	{
		this->pWriter = INetService::instance();
	}
	this->CreateThread();
}

int IPrinter::getTypte()
{
	return this->ctype;
}

bool IPrinter::setType(int type)
{
	if(type < 0 || type > 3)
	{
		return false;
	}

	this->ctype = type;
	return true;
}

int IPrinter::getSpeed()
{
	return this->cspeed;
}

bool IPrinter::setSpeed(int speed)
{
	this->cspeed = speed;
}

bool IPrinter::setIP(char* ip)
{
	strncpy(this->cip, ip, sizeof(this->cip));
}

bool IPrinter::putfile(const char* file)
{
	int fd = open(file, O_RDONLY);
	if(fd <= 0)
	{
		errorf("open [%s] failed!\n", file);
		return false;
	}
	char buf[1024]="";
	int len;
	while((len = read(fd, buf, sizeof(buf))) > 0)
	{
		infof("len = %d\n", len);
		while(this->put(buf, len) == false)
		{
			tracepoint();
			sleep(1);
		}
	}
	if(len == 0)
	{
		infof("read file end!\n");
	}
	else
	{
		errorf("something is wrong!\n");
	}

	close(fd);
	return true;
}
bool IPrinter::put(const char* dat,int len)
{
	if(this->left() < len || len == 0)
	{
		return false;
	}
	
	for(int i=0; i < len; i++)
	{
		this->cbuf[this->wr++] = dat[i];
		wr = wr & (MAX_LEN - 1);		
	}

	infof("------wr = %d\n", this->wr);
	
	if(this->wr == this->rd)
	{
		m_mutex.Enter();
		this->fullflag=true;//加锁处理
		m_mutex.Leave();
	}
		
	return true;
}

int IPrinter::left(void)
{
	if(this->wr > this->rd)
	{
		return (MAX_LEN - this->wr + this->rd);
	}
	else if(this->wr < this->rd)
	{
		return (this->rd - this->wr);
	}
	else
	{
		return this->fullflag == true ? 0 : MAX_LEN;
	}
}


int IPrinter::showbuf(char* dat, int len)
{
	int i = 0;
	if (dat == NULL || (this->wr == this->rd && this->fullflag==false))
		return 0;
		
	for(i=0; i < len; i++)
	{
		if((this->rd+i & (MAX_LEN - 1)) == this->wr)
		{
			break;
		}
		dat[i]= this->cbuf[this->rd+i & (MAX_LEN -1)];
	}
	return i;

}

static char tmp[2048];

void IPrinter::ThreadProc()
{
	int i;
	if(this->mtype == 0)//输入，需要读取输入打印机数据和状态
	{
		char tmp[128]="";
		while(1)
		{
			//sleep(1);
			//continue;
			//bzero(tmp, sizeof(tmp));
			tracepoint();
			int len = read(this->fd, tmp, sizeof(tmp));
			debugf("read len = %d, 1st is [%c]\n", len, tmp[0]);
			if(len > 0)
			{
				while(0)//(this->pWriter->write(tmp, len) == false)
				{
				//	tracepoint();
				//	sleep(1);
				}
				infof("===%p===\n", this->pWriter);
				this->pWriter->write(tmp, len);
			}
		}
	}
	else//输出，给打印机发送数据时，还需要读取打印机的状态
	{
		while(this->loopflag)
		{
			//tracepoint();
			while(this->mtype == 1 &&  this->loopflag && this->left() < MAX_LEN)
			{
				tracepoint();
				int send_len = MAX_LEN - this->left();
				//printf("%c", this->cbuf[this->rd]);
				int len;
				for(i=0;i<2048;i++)
				{
					tmp[i] = this->cbuf[this->rd++];
					this->rd = this->rd & (MAX_LEN -1);
					if((i+1) == send_len)
						break;
				}
				infof("the len is %d\n", i+1);
				len = write(this->fd, (const char*)tmp, i+1);
				if(len != (i+1))
				{
					errorf("write failed!len = %d, fd = %d\n", len, this->fd);
				}
				infof("rd = %d, send_len = %d\n", this->rd, send_len);
				//this->rd = (this->rd + send_len) & (MAX_LEN -1);
				//this->rd = this->rd & (MAX_LEN - 1);
				m_mutex.Enter();
				this->fullflag=false;// 待加锁处理
				m_mutex.Leave();
				//usleep(1000*1000);//测试代码
			}
			//读取数据
			sleep(1);
		}
	}

}