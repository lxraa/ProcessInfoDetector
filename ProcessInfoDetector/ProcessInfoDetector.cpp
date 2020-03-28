// ProcessInfoDetector.cpp: 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "ProcessInfoDetector.h"
#include "DetectorConsole.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为以下项中的第一个语句:
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CProcessInfoDetectorApp

BEGIN_MESSAGE_MAP(CProcessInfoDetectorApp, CWinApp)
END_MESSAGE_MAP()


// CProcessInfoDetectorApp 构造

CProcessInfoDetectorApp::CProcessInfoDetectorApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CProcessInfoDetectorApp 对象

CProcessInfoDetectorApp theApp;



DetectorConsole *p_main_dlg;
typedef DWORD(*show_dialog)(LPARAM lp_data);
DWORD WINAPI ShowDialog(LPARAM lp_data) {
	p_main_dlg = new DetectorConsole();
	p_main_dlg->DoModal();
	delete p_main_dlg;
	FreeLibraryAndExitThread(theApp.m_hInstance, 1);
	return TRUE;
}

// CProcessInfoDetectorApp 初始化

BOOL CProcessInfoDetectorApp::InitInstance()
{
	CWinApp::InitInstance();
	//HANDLE h_thread = ::CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ShowDialog, NULL, NULL, NULL);

	//this->hideMyself();
	LPTHREAD_START_ROUTINE new_show_dialog = NULL;

	if (this->new_instance != NULL) {
		
	#if defined(_WIN64)
		new_show_dialog = (LPTHREAD_START_ROUTINE)(((DWORD64)ShowDialog) - ((DWORD64)theApp.m_hInstance) + ((DWORD64)this->new_instance));
	#else
		new_show_dialog = (LPTHREAD_START_ROUTINE)(((DWORD32)ShowDialog) - ((DWORD32)theApp.m_hInstance) + ((DWORD32)this->new_instance));
	#endif
		DWORD_PTR t1 = (DWORD_PTR)ShowDialog;
		DWORD_PTR t2 = ((DWORD_PTR)theApp.m_hInstance);
		DWORD_PTR tmp = t1 - t2;
		tmp = tmp + (DWORD_PTR)this->new_instance;
		new_show_dialog = (LPTHREAD_START_ROUTINE)tmp;
		HANDLE h_thread = ::CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)new_show_dialog, NULL, NULL, NULL);
		return FALSE;
	}
	else {
		MessageBox(NULL, TEXT("模块隐藏失败"), TEXT("1"), MB_OK);
		HANDLE h_thread = ::CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ShowDialog, NULL, NULL, NULL);
		return TRUE;
	}
	
}

VOID CProcessInfoDetectorApp::hideMyself() {
	PIMAGE_DOS_HEADER p_dos = (PIMAGE_DOS_HEADER)theApp.m_hInstance;
	PIMAGE_NT_HEADERS p_nt = (PIMAGE_NT_HEADERS)((DWORD_PTR)theApp.m_hInstance + p_dos->e_lfanew);
	PVOID mem = VirtualAlloc(0, p_nt->OptionalHeader.SizeOfImage, MEM_COMMIT ,PAGE_EXECUTE_READWRITE);
	if (NULL == mem) {
		MessageBox(NULL, TEXT("VirtualAlloc failed"), TEXT("err"), MB_OK);
		return;
	}
	memcpy(mem, (PVOID)theApp.m_hInstance, p_nt->OptionalHeader.SizeOfImage);
	//重定位表
	PIMAGE_BASE_RELOCATION rebase_table = (PIMAGE_BASE_RELOCATION)((DWORD_PTR)mem + p_nt->OptionalHeader.DataDirectory[5].VirtualAddress);
	
	DWORD n = 0;
	DWORD_PTR base = (DWORD_PTR)mem;
	signed long long offset = (DWORD_PTR)mem - (DWORD_PTR)theApp.m_hInstance;
	if (offset == 0) {

	}
	typedef struct RELOCATIONITEM
	{
		WORD value : 12;
		WORD attr : 4;

	} *PRELOCATIONITEM;
	PRELOCATIONITEM   r_item;
	DWORD_PTR *item;
	while (TRUE) {
		if (rebase_table->SizeOfBlock == 0) {
			break;
		}
		
		r_item = (PRELOCATIONITEM)((PBYTE)rebase_table + 2 * sizeof(DWORD));
		n = (rebase_table->SizeOfBlock - 2 * sizeof(DWORD)) / 2;
		for (int i = 0; i < n; i++) {
			if (3 == r_item[i].attr) {
				item = (DWORD_PTR *)(base + rebase_table->VirtualAddress + r_item[i].value);
				*item = (*item + offset);
			}
		}
		rebase_table = (PIMAGE_BASE_RELOCATION)((PBYTE)rebase_table + rebase_table->SizeOfBlock);
	}
	this->new_instance = (DWORD_PTR)mem;
}