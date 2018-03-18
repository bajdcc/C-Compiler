#pragma once
#include "UITextView.h"

// 最大可撤消次数
#define UNDO_DESCRIP_BUF		32
#define	MAX_BUF_SIZE			1024
#define LF_BOOKMARK(id)		    (LF_BOOKMARK_FIRST << id)

enum LINEFLAGS
{
    LF_BOOKMARK_FIRST			= 0x00000001L,
    LF_EXECUTION				= 0x00010000L,
    LF_BREAKPOINT				= 0x00020000L,
    LF_COMPILATION_ERROR		= 0x00040000L,
    LF_BOOKMARKS	        	= 0x00080000L,
    LF_INVALID_BREAKPOINT       = 0x00100000L
};

enum CRLFSTYLE
{
    CRLF_STYLE_AUTOMATIC	= -1,
    CRLF_STYLE_DOS			= 0,
    CRLF_STYLE_UNIX			= 1,
    CRLF_STYLE_MAC			= 2
};

enum
{
    CE_ACTION_UNKNOWN		= 0,
    CE_ACTION_PASTE			= 1,
    CE_ACTION_DELSEL		= 2,
    CE_ACTION_CUT			= 3,
    CE_ACTION_TYPING		= 4,
    CE_ACTION_BACKSPACE		= 5,
    CE_ACTION_INDENT		= 6,
    CE_ACTION_DRAGDROP		= 7,
    CE_ACTION_REPLACE		= 8,
    CE_ACTION_DELETE		= 9,
    CE_ACTION_AUTOINDENT	= 10
    //	...
    //	Expandable: user actions allowed
};

class IUpdateContext
{
public:
    virtual void RecalcPoint(CPoint &ptPoint) = 0;
};


// CUITextBuffer 命令目标

class AFX_EXT_CLASS CUITextBuffer : public CCmdTarget
{
	DECLARE_DYNAMIC(CUITextBuffer)

private:
    BOOL m_bInit;
    BOOL m_bReadOnly;
    BOOL m_bModified;
    int m_nCRLFMode;
    BOOL m_bCreateBackupFile;
    int m_nUndoBufSize;
    int FindLineWithFlag(DWORD dwFlag);

protected:
#pragma pack(push, 1)
    //	Nested class declarations
    struct SLineInfo
    {
        TCHAR	*m_pcLine;
        int		m_nLength, m_nMax;
        DWORD	m_dwFlags;

        SLineInfo() { memset(this, 0, sizeof(SLineInfo)); };
    };

    enum
    {
        UNDO_INSERT			= 0x0001,
        UNDO_BEGINGROUP		= 0x0100
    };

    //	[JRT] Support For Descriptions On Undo/Redo Actions
    struct SUndoRecord
    {
        DWORD	m_dwFlags;

        CPoint	m_ptStartPos, m_ptEndPos;			//	Block of text participating
        int		m_nAction;							//	For information only: action type

    private:
        //	TCHAR	*m_pcText;
        //	Since in most cases we have 1 character here, 
        //	we should invent a better way. Note: 2 * sizeof(WORD) <= sizeof(TCHAR*)
        //
        //	Here we will use the following trick: on Win32 platforms high-order word
        //	of any pointer will be != 0. So we can store 1 character strings without
        //	allocating memory.
        //

        union
        {
            TCHAR	*m_pszText;		//	For cases when we have > 1 character strings
            TCHAR	m_szText[2];	//	For single-character strings
        };

    public:
        //	constructor/destructor for this struct
        SUndoRecord() { memset(this, 0, sizeof(SUndoRecord)); };

        void SetText(LPCTSTR pszText);
        void FreeText();

        LPCTSTR GetText() const
        {
            if (HIWORD((DWORD) m_pszText) != 0)
                return m_pszText;
            return m_szText;
        };
    };
#pragma pack(pop)

    class CInsertContext : public IUpdateContext
    {
    public:
        CPoint m_ptStart, m_ptEnd;
        virtual void RecalcPoint(CPoint &ptPoint);
    };

    class CDeleteContext : public IUpdateContext
    {
    public:
        CPoint m_ptStart, m_ptEnd;
        virtual void RecalcPoint(CPoint &ptPoint);
    };

    //	Lines of text
    CArray <SLineInfo, SLineInfo&> m_aLines;

    //	Undo
    CArray <SUndoRecord, SUndoRecord&> m_aUndoBuf;
    int m_nUndoPosition;
    int m_nSyncPosition;
    BOOL m_bUndoGroup, m_bUndoBeginGroup;

    //	Connected views
    CList <CUITextView *, CUITextView *> m_lpViews;

    //	Helper methods
    void InsertLine(LPCTSTR pszLine, int nLength = -1, int nPosition = -1);
    void AppendLine(int nLineIndex, LPCTSTR pszChars, int nLength = -1);

    //	Implementation
    BOOL InternalInsertText(CUITextView *pSource, int nLine, int nPos, LPCTSTR pszText, int &nEndLine, int &nEndChar);
    BOOL InternalDeleteText(CUITextView *pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos);

    //	[JRT] Support For Descriptions On Undo/Redo Actions
    void AddUndoRecord(BOOL bInsert, const CPoint &ptStartPos, const CPoint &ptEndPos, 
        LPCTSTR pszText, int nActionType = CE_ACTION_UNKNOWN);

    //	Overridable: provide action description
    virtual BOOL GetActionDescription(int nAction, CString &desc);

    // Operations
public:
    //	Construction/destruction code
    CUITextBuffer();
    ~CUITextBuffer();

    //	Basic functions
    BOOL InitNew(int nCrlfStyle = CRLF_STYLE_DOS);
    BOOL LoadFromFile(LPCTSTR pszFileName, int nCrlfStyle = CRLF_STYLE_AUTOMATIC);
    BOOL SaveToFile(LPCTSTR pszFileName, int nCrlfStyle = CRLF_STYLE_AUTOMATIC, BOOL bClearModifiedFlag = TRUE);
    void FreeAll();

    //	'Dirty' flag
    virtual void SetModified(BOOL bModified = TRUE);
    BOOL IsModified() const;

    //	Connect/disconnect views
    void AddView(CUITextView *pView);
    void RemoveView(CUITextView *pView);

    //	Text access functions
    int GetLineCount();
    int GetLineLength(int nLine);
    LPTSTR GetLineChars(int nLine);
    DWORD GetLineFlags(int nLine);
    int GetLineWithFlag(DWORD dwFlag);
    void SetLineFlag(int nLine, DWORD dwFlag, BOOL bSet, BOOL bRemoveFromPreviousLine = TRUE);
    void GetText(int nStartLine, int nStartChar, int nEndLine, int nEndChar, CString &text, LPCTSTR pszCRLF = NULL);

    //	Attributes
    int GetCRLFMode();
    void SetCRLFMode(int nCRLFMode);
    BOOL GetReadOnly() const;
    void SetReadOnly(BOOL bReadOnly = TRUE);

    //	Text modification functions
    BOOL InsertText(CUITextView *pSource, int nLine, int nPos, LPCTSTR pszText, int &nEndLine, int &nEndChar, int nAction = CE_ACTION_UNKNOWN);
    BOOL DeleteText(CUITextView *pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos, int nAction = CE_ACTION_UNKNOWN);

    //	Undo/Redo
    BOOL CanUndo();
    BOOL CanRedo();
    BOOL Undo(CPoint &ptCursorPos);
    BOOL Redo(CPoint &ptCursorPos);

    //	Undo grouping
    void BeginUndoGroup(BOOL bMergeWithPrevious = FALSE);
    void FlushUndoGroup(CUITextView *pSource);

    //	Browse undo sequence
    POSITION GetUndoDescription(CString &desc, POSITION pos = NULL);
    POSITION GetRedoDescription(CString &desc, POSITION pos = NULL);

    //	Notify all connected views about changes in text
    void UpdateViews(CUITextView *pSource, IUpdateContext *pContext,
        DWORD dwUpdateFlags, int nLineIndex = -1);

    // More bookmarks
    int FindNextBookmarkLine(int nCurrentLine = 0);
    int FindPrevBookmarkLine(int nCurrentLine = 0);

protected:
	DECLARE_MESSAGE_MAP()
};


