#ifndef FD_NPU_H
#define FD_NPU_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <string>
#include <string.h>

class fd_npu {
private:
	std::string resourcePath = "/data/";

	fd_npu(const char *path = NULL);
	~fd_npu();

public:
	/**
	 * 获取fd_npu单例对象
	 * 
	 * @prarm path 初始化对象所需资源路径，默认路径 “/data/”
	 * */
	static fd_npu *_GetInstance(const char* path = NULL);
	
	/**
	 * 调用nlp预测意图
	 * 
	 * @prarm asr 会话原话，需予语义解析原文
	 * @return string nlp预测结果action
	 * */
	std::string handleNlpResult(std::string asr);

	void clearDialog();
	std::string handleNlpResultWithEvent(std::string event);

};

#endif //FD_NPU_H
