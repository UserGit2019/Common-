//使应用程序只能运行一个实例
//
//Windows是多进程操作系统，框架生成的应用程序可以多次运行，形成多个运行实例。
//但在有些情况下为保证应用程序的安全运行，要求程序只能运行一个实例，比如程
//序要使用只能被一个进程单独使用的特殊硬件（例如调制解调器）时，必须限制程
//序只运行一个实例。
//
//这里涉及两个基本的问题，一是在程序的第二个实例启动时，如何发现该程序已有
//一个实例在运行，而是如何将第一个实例激活，而第二个实例退出。
//
//对于第一个问题，可以通过给应用程序设置信号量，实例启动时首先检测该信号量，
//如已存在，则说明程序已运行一个实例。
//
//第二个问题的难点是获取第一个实例的主窗对象指针或句柄，然后便可用
//SetForegroundWindow来激活。虽然FindWindow函数能寻找正运行着的窗口，但该函
//数要求指明所寻找窗口的标题或窗口类名，不是实现通用方法的途径。
//我们可以用Win32 SDK函数SetProp来给应用程序主窗设置一个特有的标记。
//用GetDesktopWindow 可以获取Windows系统主控窗口对象指针或句柄，所有应用程
//序主窗都可看成该窗口的子窗口，即可用GetWindow函数来获得它们的对象指针或句
//柄。用Win32 SDK函数GetProp查找每一应用程序主窗是否包含有我们设置的特定标
//记便可确定它是否我们要寻找的第一个实例主窗。使第二个实例退出很简单，只要
//让其应用程序对象的InitInstance函数返回FALSE即可。此外，当主窗口退出时，应
//用RemoveProp函数删除我们为其设置的标记。
//
//下面的InitInstance、OnCreate和OnDestroy函数代码将实现上述的操作：

BOOL CEllipseWndApp::InitInstance()
{
	// 用应用程序名创建信号量 
	HANDLE hSem = CreateSemaphore(NULL, 1, 1, m_pszExeName);

	// 信号量已存在？ 
	// 信号量存在，则程序已有一个实例运行 
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// 关闭信号量句柄 
		CloseHandle(hSem);
		// 寻找先前实例的主窗口 
		HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
		while (::IsWindow(hWndPrevious))
		{
			// 检查窗口是否有预设的标记? 
			// 有，则是我们寻找的主窗 
			if (::GetProp(hWndPrevious, m_pszExeName))
			{
				// 主窗口已最小化，则恢复其大小 
				if (::IsIconic(hWndPrevious))
					::ShowWindow(hWndPrevious, SW_RESTORE);

				// 将主窗激活 
				::SetForegroundWindow(hWndPrevious);

				// 将主窗的对话框激活 
				::SetForegroundWindow(
					::GetLastActivePopup(hWndPrevious));

				// 退出本实例 
				return FALSE;
			}
			// 继续寻找下一个窗口 
			hWndPrevious = ::GetWindow(hWndPrevious, GW_HWNDNEXT);
		}
		// 前一实例已存在，但找不到其主窗 
		// 可能出错了 
		// 退出本实例 
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization 
	// If you are not using these features and wish to reduce the size 
	// of your final executable, you should remove from the following 
	// the specific initialization routines you do not need. 
#ifdef _AFXDLL 
	Enable3dControls(); // Call this when using MFC in a shared DLL 
#else 
	Enable3dControlsStatic();// Call this when linking to MFC statically 
#endif 

	CEllipseWndDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is 
		// dismissed with OK 
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is 
		// dismissed with Cancel 
	}
	// Since the dialog has been closed, return FALSE so that we exit the 
	// application, rather than start the application's message pump. 
	return FALSE;
}

int CEllipseWndDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	// 设置寻找标记 
	::SetProp(m_hWnd, AfxGetApp()->m_pszExeName, (HANDLE)1);
	return 0;
}

void CEllipseWndDlg::OnDestroy()
{
	CDialog::OnDestroy();
	// 删除寻找标记 
	::RemoveProp(m_hWnd, AfxGetApp()->m_pszExeName);
}


