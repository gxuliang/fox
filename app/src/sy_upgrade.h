#ifndef _IUPGRADE_H_
#define _IUPGRADE_H_


class IUpgrade
{
public:
	bool putfile(const char* file);//升级
	bool fileChecked(void);

private:
	bool flag;
};

#endif