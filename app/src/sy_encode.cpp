/*
 * sy_encode.cpp
 *
 *  Created on: 2013-4-15
 *      Author: xul
 */


#include "base/sy_types.h"
#include "base/sy_guard.h"
#include "inc_comm/com_encode.h"
#include "inc_comm/com_command.h"
#include "sy_encode.h"

int CEncode::w_fd = 0;
int CEncode::r_fd = 0;
CMutex CEncode::m_mutex(CMutex::mutexRecursive);
CamGlobal CEncode::mcamglo;

static Commder sCmmd;

int sy_connvert(std::string str, int n)
{
	int ret = 0,i;
	std::string p[]={"1080P", "720P", "D1", "VGA"};

	switch(n)
	{
	case CovPX:

		for(i=0; i < sizeof(p)/sizeof(p[0]); i++)
		{
			if(p[i] == str)
				ret = i;
				break;
		}
		break;
	default:
		break;
	}
	return ret;
}
CEncode::CEncode()
{

}

bool CEncode::setEncode(const CConfigTable &table, const uint channel = 0)
{
	CGuard guard(m_mutex);
	memset(&sCmmd, 0, sizeof(Commder));
	sCmmd.cmdtype = Cmdset;
	//sCmmd.cmd = Cndencode;
	sCmmd.para.encoder.mitype = mtype;
	sCmmd.para.encoder.vdenable = table["VideoEnable"].asBool();
	sCmmd.para.encoder.vicom.vi_type = vt_ordinary;//table["Video"]["type"].asString();
	sCmmd.para.encoder.vicom.vi_px = ViPx(sy_connvert(table["Video"]["PX"].asString(), CovPX));
	sCmmd.para.encoder.vicom.vi_vc = ViComp(sy_connvert(table["Video"]["Compression"].asString(), CovComp));
	sCmmd.para.encoder.vicom.fps = table["Video"]["FPS"].asUInt();
	sCmmd.para.encoder.vicom.gop = table["Video"]["GOP"].asUInt();
	sCmmd.para.encoder.vicom.vi_strm = ViSm(sy_connvert(table["Video"]["FPS"].asString(), ConStrm));
	sCmmd.para.encoder.vicom.stream = table["Video"]["Stream"].asUInt();

	int ret = write(w_fd, &sCmmd, sizeof(Commder));
	return true;
}

bool CEncode::setCamGloble(const CConfigTable &table)
{
	CGuard guard(m_mutex);
	memset(&sCmmd, 0, sizeof(Commder));
	sCmmd.cmdtype = Cmdset;
	sCmmd.cmd = Cmdcamg;
	sCmmd.para.camglo.Brightness = table["Brightness"].asUInt();
	sCmmd.para.camglo.Contrast = table["Contrast"].asUInt();
	sCmmd.para.camglo.Chromaticity = table["Chromaticity"].asUInt();
	sCmmd.para.camglo.Saturation = table["Saturation"].asUInt();
	//剩下的参数以后再写

	int ret = write(w_fd, &sCmmd, sizeof(Commder));
	return true;
}

bool CEncode::config(const char* wpath, const char* rpath)
{

	w_fd = open(wpath, O_WRONLY);
	r_fd = open(rpath, O_RDONLY);
	return ((w_fd > 0 && r_fd > 0) ? true : false);
}
