// SetPara.cpp : implementation file
//

#include "stdafx.h"
#include "Robot.h"
#include "SetPara.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SetPara dialog


SetPara::SetPara(CWnd* pParent /*=NULL*/)
	: CDialog(SetPara::IDD, pParent)
{
	//{{AFX_DATA_INIT(SetPara)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_Height = m_CurPara.height;
	m_Width = m_CurPara.width;
	m_Cross = m_CurPara.propC;
	m_Mutation = m_CurPara.propM;
	m_PopSize = m_CurPara.pSize;
	m_GenNum = m_CurPara.T;
}


void SetPara::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SetPara)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_EDITHEIGHT, m_Height);
	DDV_MinMaxInt(pDX,m_Height,1,100);
	DDX_Text(pDX, IDC_EDITWIDTH, m_Width);
	DDV_MinMaxInt(pDX,m_Width,1,100);
	DDX_Text(pDX, IDC_EDITCROSS, m_Cross);
	DDV_MinMaxDouble(pDX,m_Cross,0,1.0);
	DDX_Text(pDX, IDC_EDITMUTATION, m_Mutation);
	DDV_MinMaxDouble(pDX,m_Mutation,0,1.0);
	DDX_Text(pDX, IDC_EDITPOP, m_PopSize);
	DDV_MinMaxInt(pDX,m_PopSize,1,500);
	DDX_Text(pDX, IDC_EDITGENNUM, m_GenNum);
	DDV_MinMaxInt(pDX,m_GenNum,1,3000);
	m_Length = ((CButton *)GetDlgItem(IDC_LENGTH))->GetCheck();
	m_Smooth = ((CButton *)GetDlgItem(IDC_SMOOTH))->GetCheck();
	m_Safe = ((CButton *)GetDlgItem(IDC_SAFE))->GetCheck();
}




BEGIN_MESSAGE_MAP(SetPara, CDialog)
	//{{AFX_MSG_MAP(SetPara)
	ON_EN_CHANGE(IDC_EDITCROSS, OnChangeEditcross)
	ON_EN_CHANGE(IDC_EDITGENNUM, OnChangeEditgennum)
	ON_EN_CHANGE(IDC_EDITHEIGHT, OnChangeEditheight)
	ON_EN_CHANGE(IDC_EDITMUTATION, OnChangeEditmutation)
	ON_EN_CHANGE(IDC_EDITPOP, OnChangeEditpop)
	ON_EN_CHANGE(IDC_EDITWIDTH, OnChangeEditwidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SetPara message handlers

void SetPara::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void SetPara::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void SetPara::OnChangeEditcross() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	this->UpdateData();
}

void SetPara::OnChangeEditgennum() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	this->UpdateData();
}

void SetPara::OnChangeEditheight() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	this->UpdateData();
}

void SetPara::OnChangeEditmutation() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	this->UpdateData();
}

void SetPara::OnChangeEditpop() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	this->UpdateData();
}

void SetPara::OnChangeEditwidth() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	this->UpdateData();
}


BOOL SetPara::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	if(m_CurPara.length == 1)
		((CButton *)GetDlgItem(IDC_LENGTH))->SetCheck(TRUE);
	else
		((CButton *)GetDlgItem(IDC_LENGTH))->SetCheck(FALSE);
	if(m_CurPara.smooth == 1)
		((CButton *)GetDlgItem(IDC_SMOOTH))->SetCheck(TRUE);
	else
		((CButton *)GetDlgItem(IDC_SMOOTH))->SetCheck(FALSE);
	if(m_CurPara.safe == 1)
		((CButton *)GetDlgItem(IDC_SAFE))->SetCheck(TRUE);
	else
		((CButton *)GetDlgItem(IDC_SAFE))->SetCheck(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
