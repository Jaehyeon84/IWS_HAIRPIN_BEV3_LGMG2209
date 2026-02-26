
#include <afxwin.h>
class CDrawView :
	public CView
{
public:
	CDrawView();
	~CDrawView();
	DECLARE_DYNCREATE(CDrawView);
	CIWSDoc* GetDocument() const;
	// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	DECLARE_MESSAGE_MAP()
};
