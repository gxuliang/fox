/*
 * sy_file.h
 *
 *  Created on: 2013-3-20
 *      Author: xul
 */

#ifndef SY_FILE_H_
#define SY_FILE_H_

struct FileInternal
{
	FILE *m_file;				///< 标准库的文件结构指针。
	unsigned char *m_buffer;	///< 文件数据缓冲，Load和UnLoad接口使用。
	unsigned int m_length;		///< 文件长度。
	unsigned int m_position;		///< 文件读写指针。
	FSOperations* m_opt;		///< 文件系统函数。
};

struct FileFindInternal
{
	long m_handle;
	FileInfo m_fileInfo;
	FSOperations* m_opt;		///< 文件系统函数。
	std::string m_path;			///< 查找路径。
};



#endif /* SY_FILE_H_ */
