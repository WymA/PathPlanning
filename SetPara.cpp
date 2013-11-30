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
	m_Height = cur_parameter.height;
	m_Width = cur_parameter.width;
	m_Cross = cur_parameter.pop_cross_rate;
	m_Mutation = cur_parameter.pop_mutation_rate;
	m_PopSize = cur_parameter.pSize;
	m_GenNum = cur_parameter.total_gen;

	pre_state2 = 0 ;
	pre_state = 0 ;
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

	m_Length = ((CButton *)GetDlgItem(IDC_LENGTH))->GetCheck() |
		((CButton *)GetDlgItem(IDC_LENGTH2))->GetCheck() ;
	m_Smooth = ((CButton *)GetDlgItem(IDC_SMOOTH))->GetCheck() |
		((CButton *)GetDlgItem(IDC_SMOOTH2))->GetCheck() ;
	m_Safe = ((CButton *)GetDlgItem(IDC_SAFE))->GetCheck() | 
		((CButton *)GetDlgItem(IDC_SAFE2))->GetCheck() ;
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
//	ON_BN_CLICKED(IDC_LENGTH, &SetPara::OnBnClickedLength)
ON_BN_CLICKED(IDC_LENGTH, &SetPara::OnBnClickedLength)
ON_BN_CLICKED(IDC_SMOOTH, &SetPara::OnBnClickedSmooth)
ON_BN_CLICKED(IDC_SAFE, &SetPara::OnBnClickedSafe)
ON_BN_CLICKED(IDC_LENGTH2, &SetPara::OnBnClickedLength2)
ON_BN_CLICKED(IDC_SMOOTH2, &SetPara::OnBnClickedSmooth2)
ON_BN_CLICKED(IDC_SAFE2, &SetPara::OnBnClickedSafe2)
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
	if( cur_parameter.length == TRUE ){

		((CButton *)GetDlgItem(IDC_LENGTH))->SetCheck(TRUE);
		pre_state = IDC_LENGTH ;
	}else{

		((CButton *)GetDlgItem(IDC_SMOOTH))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_SAFE2))->SetCheck(TRUE);

		pre_state = IDC_SMOOTH ;
		pre_state2 = IDC_SAFE2 ;
		return TRUE ;
	}

	if ( cur_parameter.smooth ){

		((CButton *)GetDlgItem(IDC_SMOOTH2))->SetCheck(TRUE);
		pre_state2 = IDC_SMOOTH2 ;
	}else {

		((CButton *)GetDlgItem(IDC_SAFE2))->SetCheck(TRUE);
		pre_state2 = IDC_SAFE2 ;
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


//void SetPara::OnBnClickedLength()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}


void SetPara::OnBnClickedLength()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton *)GetDlgItem(IDC_LENGTH2))->GetCheck() == BST_CHECKED ) {

		((CButton *)GetDlgItem(IDC_LENGTH))->SetCheck(FALSE) ;
		((CButton *)GetDlgItem(pre_state))->SetCheck(TRUE) ;
	}else
		pre_state = IDC_LENGTH ;
}


void SetPara::OnBnClickedSmooth()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton *)GetDlgItem(IDC_SMOOTH2))->GetCheck() == BST_CHECKED ) {

		((CButton *)GetDlgItem(IDC_SMOOTH))->SetCheck(FALSE) ;
		((CButton *)GetDlgItem(pre_state))->SetCheck(TRUE) ;
	}else
		pre_state = IDC_SMOOTH ;
}


void SetPara::OnBnClickedSafe()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton *)GetDlgItem(IDC_SAFE2))->GetCheck() == BST_CHECKED ) {

		((CButton *)GetDlgItem(IDC_SAFE))->SetCheck(FALSE) ;
		((CButton *)GetDlgItem(pre_state))->SetCheck(TRUE) ;
	}else 
		pre_state = IDC_SAFE ;
}


void SetPara::OnBnClickedLength2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton *)GetDlgItem(IDC_LENGTH))->GetCheck() == BST_CHECKED ) {

		((CButton *)GetDlgItem(IDC_LENGTH2))->SetCheck(FALSE) ;
		((CButton *)GetDlgItem(pre_state2))->SetCheck(TRUE) ;
	}else
		pre_state2 = IDC_LENGTH2 ;
}


void SetPara::OnBnClickedSmooth2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton *)GetDlgItem(IDC_SMOOTH))->GetCheck() == BST_CHECKED ) {

		((CButton *)GetDlgItem(IDC_SMOOTH2))->SetCheck(FALSE) ;
		((CButton *)GetDlgItem(pre_state2))->SetCheck(TRUE) ;
	}else
		pre_state2 = IDC_SMOOTH2 ;
}


void SetPara::OnBnClickedSafe2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton *)GetDlgItem(IDC_SAFE))->GetCheck() == BST_CHECKED ) {

		((CButton *)GetDlgItem(IDC_SAFE2))->SetCheck(FALSE) ;
		((CButton *)GetDlgItem(pre_state2))->SetCheck(TRUE) ;
	}else
		pre_state2 = IDC_SAFE2 ;
}
