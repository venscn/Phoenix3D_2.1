// PX2E_MainFrame.cpp

#include "PX2E_MainFrame.hpp"
#include "PX2E_RenderView.hpp"
#include "PX2E_ResView.hpp"
#include "PX2E_ProjView.hpp"
#include "PX2E_StartView.hpp"
#include "PX2E_TimeLineView.hpp"
#include "PX2E_TopView.hpp"
#include "PX2E_InspView.hpp"
#include "PX2E_RenderView_Cont.hpp"
#include "PX2E_NirMan.hpp"
#include "PX2wxDockArt.hpp"
#include "PX2wxAui.hpp"

#include "PX2EngineLoop.hpp"
#include "PX2Edit.hpp"
#include "PX2ScriptManager.hpp"
#include "PX2Project.hpp"
#include "PX2EventWorld.hpp"
#include "PX2DlgCreateProject.hpp"
#include "PX2ObjectInspector.hpp"
#include "PX2SimulationEventType.hpp"
#include "PX2EditEventType.hpp"
#include "PX2NirvanaEventType.hpp"
#include "PX2EditParams.hpp"
#include "PX2Format.hpp"

using namespace PX2Editor;
using namespace PX2;

const int sID_ENGINELOOPTIMER = PX2_EDIT_GETID;

BEGIN_EVENT_TABLE(E_MainFrame, wxFrame)
EVT_TIMER(sID_ENGINELOOPTIMER, E_MainFrame::OnTimer)
END_EVENT_TABLE();
//----------------------------------------------------------------------------
E_MainFrame::E_MainFrame(const std::string &title, int xPos, int yPos,
	int width, int height) :
	wxFrame((wxFrame*)0, -1, title, wxPoint(xPos, yPos), wxSize(width, height)),
	mIsInitlized(false),
	mAuiManager(0),
	mTopView(0),
	mRenderViewScene(0),
	mRenderViewLogic(0),
	mProjView(0),
	mIsCrossCursor(false)
{
	mPerspConfigName = "nirvana1.0.0.config";

	PX2_EW.ComeIn(this);
}
//----------------------------------------------------------------------------
E_MainFrame::~E_MainFrame()
{
	if (mAuiManager)
	{
		wxString strPerspective = mAuiManager->SavePerspective();

		wxFileOutputStream os(mPerspConfigName);
		wxFileConfig config;
		config.Write("Perspective", strPerspective);
		config.Save(os);
		os.Close();

		mAuiManager->UnInit();
		delete mAuiManager;
	}
}
//----------------------------------------------------------------------------
bool E_MainFrame::Initlize()
{
	EditParams *params = PX2_EDIT.GetEditParams();
	if (params)
	{
		mCurTheme = params->GetCurTheme();
	}

	// Aui
	mAuiManager = new wxAuiManager(this, wxAUI_MGR_DEFAULT
		| wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE );

	mAuiManager->SetArtProvider(new PX2wxDockArt());
	mAuiManager->GetArtProvider()->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 24);
	mAuiManager->GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 1);
	mAuiManager->GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE, 3);

	SetAuiManColorForTheme();

	mAuiManager->SetFlags(mAuiManager->GetFlags() | wxAUI_MGR_LIVE_RESIZE);

	mTimer.SetOwner(this, sID_ENGINELOOPTIMER);
	mTimer.Start(25);

	_CreateMenu();
	//_CreateTopView();
	_CreateMainToolBar();
	_CreateViews();
	_CreateStatusBar();

	wxFileInputStream is(mPerspConfigName);
	if (is.IsOk())
	{
		wxFileConfig config(is);
		wxString strPerspective;
		if (config.Read(wxString("Perspective"), &strPerspective))
		{
			mAuiManager->LoadPerspective(strPerspective);
		}
	}

	mAuiManager->Update();

	mIsInitlized = true;

	return true;
}
//----------------------------------------------------------------------------
void E_MainFrame::SetAuiManColorForTheme()
{
	float r = mCurTheme.tabBackColor[0] * 255.0f;
	float g = mCurTheme.tabBackColor[1] * 255.0f;
	float b = mCurTheme.tabBackColor[2] * 255.0f;
	mAuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, wxColour(r, g, b));

	mAuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_BORDER_COLOUR, wxColour(r, g, b));//44 61 91
	mAuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_SASH_COLOUR, wxColour(r, g, b));
	mAuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_GRIPPER_COLOUR, wxColour(r, g, b));

	r = mCurTheme.inactiveColor[0] * 255.0f;
	g = mCurTheme.inactiveColor[1] * 255.0f;
	b = mCurTheme.inactiveColor[2] * 255.0f;
	mAuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, wxColour(r, g, b)); //77 96 130
	mAuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(r, g, b));

	r = mCurTheme.activeColor[0] * 255.0f;
	g = mCurTheme.activeColor[1] * 255.0f;
	b = mCurTheme.activeColor[2] * 255.0f;
	mAuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR, wxColour(r, g, b));//255 242 157
	mAuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(r, g, b));
}
//----------------------------------------------------------------------------
void E_MainFrame::DoExecute(Event *event)
{
	if (EditEventSpace::IsEqual(event, EditEventSpace::NewScene))
	{
	}
	else if (EditEventSpace::IsEqual(event, EditEventSpace::LoadedScene))
	{
	}
	else if (EditEventSpace::IsEqual(event, EditEventSpace::CloseScene))
	{
	}
	else if (NirvanaEventSpace::IsEqual(event, NirvanaEventSpace::TabDrag))
	{
		wxWindow *window = event->GetData<wxWindow*>();
		_CreateView(window, "ResView", "ResView", "ResView",
			wxAuiPaneInfo().Right().BestSize(200, 200).Float());
		mAuiManager->Update();
	}
	else if (EditEventSpace::IsEqual(event, EditEventSpace::SceneNodeDrag))
	{
		int drag = event->GetData<int>();
		if (1 == drag)
		{
			SetCursor(wxCursor(wxCURSOR_SIZING));
		}
		else if (0 == drag)
		{
			SetCursor(wxCursor(wxCURSOR_ARROW));
		}
	}
	else if (EditEventSpace::IsEqual(event, EditEventSpace::EditThemeChange))
	{
		if (mAuiManager)
		{
			EditParams *params = PX2_EDIT.GetEditParams();
			if (params)
			{
				EditParams::Theme theme = params->GetCurTheme();
				mCurTheme = theme;
				mProjView->SetColorForTheme(mCurTheme);
				mTimeLineView->SetColorForTheme(mCurTheme);
				mResView->SetColorForTheme(mCurTheme);
				SetAuiManColorForTheme();
				Refresh();
			}
		}
	}
	else if (SimuES_E::IsEqual(event, SimuES_E::AddSelect) ||
		SimuES_E::IsEqual(event, SimuES_E::RemoveSelect) ||
		SimuES_E::IsEqual(event, SimuES_E::RemoveAllSelects))
	{
		_UpdateStatusSelectObject(2);
	}
	else if (EditEventSpace::IsEqual(event, EditEventSpace::SetPickPos))
	{
		const APoint &pickPos = PX2_EDIT.GetPickPos();
		_UpdateStatusPickingPos(4);
	}
}
//----------------------------------------------------------------------------
void E_MainFrame::_UpdateStatusSelectObject(int index)
{
	wxStatusBar *sb = GetStatusBar();

	std::string str;
	int numObj = PX2_SELECTION.GetNumObjects();
	if (0 == numObj)
	{
		str = PX2_LMVAL("CurSelObjNull");
	}
	else
	{
		Object *firObj = PX2_SELECTION.GetFirstObject();

		str = PX2_LMVAL("CurSelObj");
		str += " " + StringHelp::IntToString(numObj) + PX2_LMVAL("Ge");
		str += " " + PX2_LMVAL("Name") + ": " + firObj->GetName();
	}

	sb->SetStatusText(str, index);
}
//----------------------------------------------------------------------------
void E_MainFrame::_UpdateStatusSelectRes(int index)
{
	wxStatusBar *sb = GetStatusBar();
	sb->SetStatusText(PX2_LMVAL("CurSelResNull"), index);
}
//----------------------------------------------------------------------------
void E_MainFrame::_UpdateStatusPickingPos(int index)
{
	const APoint &pickPos = PX2_EDIT.GetPickPos();

	wxStatusBar *sb = GetStatusBar();

	std::string str;
	str = PX2_LMVAL("PickingPos") + "(" +
		StringHelp::FloatToString(pickPos[0]) + ", " +
		StringHelp::FloatToString(pickPos[1]) + ", " +
		StringHelp::FloatToString(pickPos[2]) + ")";

	sb->SetStatusText(str, index);
}
//----------------------------------------------------------------------------
RenderView *E_MainFrame::GetRenderViewScene()
{
	return mRenderViewScene;
}
//----------------------------------------------------------------------------
void E_MainFrame::OnTimer(wxTimerEvent& e)
{
	PX2_UNUSED(e);

	if (mIsInitlized)
	{
		PX2_EDIT.IsAltDown = wxGetKeyState(WXK_ALT);
		PX2_EDIT.IsCtrlDown = wxGetKeyState(WXK_CONTROL);
		PX2_EDIT.IsShiftDown = wxGetKeyState(WXK_SHIFT);

		PX2_ENGINELOOP.Tick();
	}
}
//----------------------------------------------------------------------------
void E_MainFrame::OnCommondItem(wxCommandEvent &e)
{
	int id = e.GetId();

	std::map<int, std::string>::iterator it = mIDScripts.find(id);

	if (it != mIDScripts.end())
	{
		std::string callStr = it->second;
		PX2_SM.CallString(callStr);
	}
}
//----------------------------------------------------------------------------
void E_MainFrame::OnNewProject()
{
	DlgCreateProject dlg(this);

	if (wxID_OK == dlg.ShowModal())
	{
		std::string name = dlg.mProjName;
		std::string projPath = dlg.mProjPath;
		int width = dlg.mProjWidth;
		int height = dlg.mProjHeight;

		std::string path = projPath + "/" + name + ".px2proj";

		PX2_EDIT.GetEditMap()->NewProject(path, name, width, height);
	}
}
//----------------------------------------------------------------------------
void E_MainFrame::OnOpenProject()
{
	wxFileDialog dlg(this,
		wxT("Open project"),
		wxEmptyString,
		wxEmptyString,
		wxT("Project (*.px2proj)|*.px2proj"));

	dlg.CenterOnParent();

	if (wxID_OK == dlg.ShowModal())
	{
		PX2_EDIT.GetEditMap()->LoadProject(dlg.GetPath());
	}
}
//----------------------------------------------------------------------------
void E_MainFrame::OnSaveProject()
{
	EditMap *map = PX2_EDIT.GetEditMap();

	Project *proj = Project::GetSingletonPtr();
	if (proj)
	{
		if (proj->GetScene())
		{
			OnSaveScene();
		}
	}

	map->SaveProject();
}
//----------------------------------------------------------------------------
void E_MainFrame::OnSaveProjectAs()
{
	// save scene first
	EditMap *map = PX2_EDIT.GetEditMap();
	Project *proj = Project::GetSingletonPtr();
	if (proj)
	{
		if (proj->GetScene() && proj->GetSceneFilename().empty())
		{
			OnSaveSceneAs();
		}
	}

	wxFileDialog dlg(this,
		wxT("Save project"),
		wxEmptyString,
		wxEmptyString,
		wxT("Project (*.px2proj)|*.px2proj"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	dlg.CenterOnParent();

	if (wxID_OK == dlg.ShowModal())
	{
		map->SaveProjectAs(dlg.GetPath());
	}
}
//----------------------------------------------------------------------------
void E_MainFrame::OnCloseProject()
{
	PX2_EDIT.GetEditMap()->CloseProject();
}
//----------------------------------------------------------------------------
void E_MainFrame::OnNewScene()
{
	EditMap *map = PX2_EDIT.GetEditMap();
	map->NewScene();
}
//----------------------------------------------------------------------------
void E_MainFrame::OnOpenScene()
{
	wxFileDialog dlg(this,
		wxT("Open scene"),
		wxEmptyString,
		wxEmptyString,
		wxT("scene (*.px2obj)|*.px2obj"));

	dlg.CenterOnParent();

	if (wxID_OK == dlg.ShowModal())
	{
		std::string strPath = dlg.GetPath();
		PX2_EDIT.GetEditMap()->LoadScene(strPath);
	}
}
//----------------------------------------------------------------------------
void E_MainFrame::OnSaveScene()
{
	std::string path;
	if (Project::GetSingletonPtr())
		path = Project::GetSingleton().GetSceneFilename();

	if (!path.empty())
	{
		PX2_EDIT.GetEditMap()->SaveScene(path.c_str());
	}
	else
	{
		wxFileDialog dlg(this,
			wxT("Save scene"),
			wxEmptyString,
			wxEmptyString,
			wxT("scene (*.px2obj)|*.px2obj"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

		dlg.CenterOnParent();

		if (wxID_OK == dlg.ShowModal())
		{
			std::string strPath = dlg.GetPath();
			PX2_EDIT.GetEditMap()->SaveScene(strPath);
		}
	}
}
//----------------------------------------------------------------------------
void E_MainFrame::OnSaveSceneAs()
{
	wxFileDialog dlg(this,
		wxT("Save scene"),
		wxEmptyString,
		wxEmptyString,
		wxT("scene (*.px2obj)|*.px2obj"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	dlg.CenterOnParent();

	if (dlg.ShowModal() == wxID_OK)
	{
		std::string strPath = dlg.GetPath();
		PX2_EDIT.GetEditMap()->SaveSceneAs(strPath);
	}
}
//----------------------------------------------------------------------------
void E_MainFrame::OnCloseScene()
{
	EditMap *map = PX2_EDIT.GetEditMap();
	if (map)
	{
		map->CloseScene();
	}
}
//----------------------------------------------------------------------------
void E_MainFrame::OnSetEditMode(int mode)
{
	PX2_EDIT.SetEditMode((Edit::EditMode)mode);
}
//----------------------------------------------------------------------------
void E_MainFrame::OnSetting()
{
	EditParams *params = PX2_EDIT.GetEditParams();
	if (params)
	{
		PX2_SELECTION.Clear();
		PX2_SELECTION.AddObject(params);
	}
	else
	{
		PX2_SELECTION.Clear();
	}
}
//----------------------------------------------------------------------------
void E_MainFrame::_CreateMenu()
{
	mMainMenuBar = new wxMenuBar();
	SetMenuBar(mMainMenuBar);

	PX2_SM.CallString("e_CreateMainMenu()");
}
//----------------------------------------------------------------------------
wxMenu *E_MainFrame::AddMainMenuItem(const std::string &title)
{
	wxMenu* menu = new wxMenu();

	mMainMenuBar->Append(menu, title);

	menu->GetWindow()->SetBackgroundColour(wxColour(234, 240, 255));

	return menu;
}
//----------------------------------------------------------------------------
wxMenuItem *E_MainFrame::AddMenuItem(wxMenu *menu, const std::string &title,
	const std::string &script)
{
	int id = PX2_EDIT_GETID;
	wxMenuItem *item = new wxMenuItem(menu, id, title);
	//item->SetBackgroundColour(wxColour(234, 240, 255));
	//item->SetTextColour(wxColour(0, 0, 0));
	menu->Append(item);

	Connect(id, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(E_MainFrame::OnCommondItem));

	mIDScripts[id] = script;

	return item;
}
//----------------------------------------------------------------------------
void E_MainFrame::AddSeparater(wxMenu *menu)
{
	wxMenuItem *item = new wxMenuItem(menu, wxID_SEPARATOR);
	item->SetBackgroundColour(wxColour(234, 240, 255));
	item->SetTextColour(wxColour(0, 0, 0));
	menu->Append(item);
}
//----------------------------------------------------------------------------
void E_MainFrame::AddTool(wxAuiToolBar *toolBar, const std::string &icon,
	std::string &script)
{
	int id = PX2_EDIT_GETID;
	toolBar->AddTool(id, "", wxBitmap(icon, wxBITMAP_TYPE_PNG));

	Connect(id, wxEVT_COMMAND_TOOL_CLICKED,
		wxCommandEventHandler(E_MainFrame::OnCommondItem));

	mIDScripts[id] = script;
}
//----------------------------------------------------------------------------
void E_MainFrame::AddToolSeparater(wxAuiToolBar *toolBar)
{
	toolBar->AddSeparator();
}
//----------------------------------------------------------------------------
void E_MainFrame::_CreateTopView()
{
	mTopView = new TopView(this);

	mAuiManager->AddPane(mTopView, wxAuiPaneInfo().
		Name(wxT("tb")).
		ToolbarPane().Gripper(false).Top().Dockable(false).PaneBorder(false).Resizable(false).
		MinSize(200, 32).MaxSize(200, 32).Resizable(false));
}
//----------------------------------------------------------------------------
void E_MainFrame::_CreateMainToolBar()
{
	wxAuiToolBar *mianToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_HORIZONTAL);

	mianToolBar->SetArtProvider(new PX2wxAuiToolBarArt());

	NirMan::GetSingleton().SetCurToolBar(mianToolBar);

	PX2_SM.CallString("e_CreateToolBarMain()");

	mianToolBar->Realize();

	mAuiManager->AddPane(mianToolBar, wxAuiPaneInfo().
		Name(wxT("maintoolbar")).
		ToolbarPane().Gripper(false).Top().Dockable(false).PaneBorder(false).Resizable(false).
		MinSize(200, 30).MaxSize(200, 30).Resizable(false));
}
//----------------------------------------------------------------------------
void E_MainFrame::_CreateViews()
{
	_CreateProjView();
	_CreateMainView();
	_CreateInsp();
	_CreateTimeLine();
}
//----------------------------------------------------------------------------
void E_MainFrame::_CreateProjView()
{
	mProjView = new ProjView(this);
	mProjView->SetColorForTheme(mCurTheme);
	_CreateView(mProjView, "ProjView", PX2_LM.GetValue("Project"),
		PX2_LM.GetValue("Project"),
		wxAuiPaneInfo().Left());
}
//----------------------------------------------------------------------------
void E_MainFrame::_CreateMainView()
{
	std::vector<WindowObj> objs;

	mStartView = new StartView(this);
	WindowObj objStart;
	objStart.TheWindow = mStartView;
	objStart.Caption = PX2_LMVAL("StartView");
	objStart.Name = "StartView";
	objs.push_back(objStart);

	RenderView_Cot *viewCont_SceneUI = new RenderView_Cot(RVT_SCENEUI, this);
	mRenderViewScene = viewCont_SceneUI->GetRenderView();
	WindowObj objRenderViewScene;
	objRenderViewScene.TheWindow = viewCont_SceneUI;
	objRenderViewScene.Caption = PX2_LMVAL("Stage");
	objRenderViewScene.Name = "Stage";
	objs.push_back(objRenderViewScene);

	RenderView_Cot *viewCont_Logic = new RenderView_Cot(RVT_LOGIC, this);
	mRenderViewLogic = viewCont_Logic->GetRenderView();
	WindowObj objLogicView;
	objLogicView.TheWindow = viewCont_Logic;
	objLogicView.Caption = PX2_LMVAL("LogicView");
	objLogicView.Name = "Logic";
	objs.push_back(objLogicView);

	mNoteBookCenter = _CreateView(objs, "Center", wxAuiPaneInfo().CenterPane(), 
		"Center", true);
}
//----------------------------------------------------------------------------
void E_MainFrame::_RefreshRenderView(bool show)
{
}
//----------------------------------------------------------------------------
void E_MainFrame::_CreateInsp()
{
	WindowObj objRes;
	mResView = new ResView(this);
	mResView->SetColorForTheme(mCurTheme);
	objRes.TheWindow = mResView;
	objRes.Caption = PX2_LM.GetValue("ResView");
	objRes.Name = "ResView";

	WindowObj objInsp;
	mInspView = new InspView(this);
	objInsp.TheWindow = mInspView;
	objInsp.Caption = PX2_LM.GetValue("InspView");
	objInsp.Name = "InspView";

	std::vector<WindowObj> objs;
	objs.push_back(objRes);
	objs.push_back(objInsp);

	_CreateView(objs, PX2_LM.GetValue("ResView"), wxAuiPaneInfo().Right(), "Right");
}
//----------------------------------------------------------------------------
void E_MainFrame::_CreateTimeLine()
{
	mTimeLineView = new TimeLineView(this);
	mTimeLineView->SetColorForTheme(mCurTheme);
	_CreateView(mTimeLineView, "TimeLine", PX2_LM.GetValue("TimeLine"), PX2_LM.GetValue("TimeLine"),
		wxAuiPaneInfo().DefaultPane());
}
//----------------------------------------------------------------------------
PX2wxAuiNotebook *E_MainFrame::_CreateView(wxWindow *window0, const std::string &name0,
	const std::string &caption0, const std::string &caption,
	wxAuiPaneInfo &paneInfo, bool isTopStyle)
{
	WindowObj obj;
	obj.TheWindow = window0;
	obj.Name = name0;
	obj.Caption = caption0;

	std::vector<WindowObj> winObjs;
	winObjs.push_back(obj);

	wxString paneName = caption;

	return _CreateView(winObjs, caption, paneInfo, paneName, isTopStyle);
}
//----------------------------------------------------------------------------
PX2wxAuiNotebook *E_MainFrame::_CreateView(std::vector<WindowObj> &objs,
	const std::string &caption,
	wxAuiPaneInfo &paneInfo,
	wxString paneName,
	bool isTopStyle)
{
	PX2wxAuiNotebook* noteBook = new PX2wxAuiNotebook(this, isTopStyle);
	PX2wxAuiTabArt *tabArt = new PX2wxAuiTabArt(isTopStyle);
	noteBook->SetArtProvider(tabArt);
	long styleFlag = 0;
	styleFlag ^= wxAUI_NB_TAB_MOVE;
	if (isTopStyle)
	{
		styleFlag ^= wxAUI_NB_WINDOWLIST_BUTTON;
		styleFlag ^= wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
		styleFlag ^= wxAUI_NB_TAB_FIXED_WIDTH;
	}
	else
	{
		styleFlag ^= wxAUI_NB_BOTTOM;
	}

	noteBook->SetWindowStyleFlag(styleFlag);
	noteBook->SetTabCtrlHeight(24);
	noteBook->Freeze();

	wxBitmap bitMap = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16));
	for (int i = 0; i < (int)objs.size(); i++)
	{
		WindowObj &obj = objs[i];

		noteBook->AddPage(obj.TheWindow, obj.Caption, false, bitMap);

		WindowObj winObj;
		winObj.Name = obj.Name;
		winObj.TheWindow = obj.TheWindow;
		winObj.NoteBook = noteBook;
		mWindowObjs[obj.Name] = winObj;
	}

	noteBook->UpdateTabsHeight();
	noteBook->Thaw();

	paneInfo.CloseButton(true).MaximizeButton(true).MinimizeButton(true)
		.PinButton(true).FloatingSize(220, 150).MinSize(100, 100).Caption(caption).Name(paneName);
	mAuiManager->AddPane(noteBook, paneInfo);

	noteBook->Refresh();

	return noteBook;
}
//----------------------------------------------------------------------------
void E_MainFrame::_CreateStatusBar()
{
	wxStatusBar *status = CreateStatusBar();

	const int numFields = 5;
	status->SetFieldsCount(numFields);

	int widths[numFields] = {0, 300, 250, -1, 300 };
	status->SetStatusWidths(numFields, widths);

	int styles[numFields] = { wxSB_FLAT, wxSB_NORMAL, wxSB_FLAT, wxSB_NORMAL, wxSB_FLAT };
	status->SetStatusStyles(numFields, styles);

	status->SetStatusText(PX2_LMVAL("WelcomeToEditor"), 1);

	_UpdateStatusSelectObject(2);
	_UpdateStatusSelectRes(3);
	_UpdateStatusPickingPos(4);

	status->SetBackgroundColour(wxColour(0, 122, 204));
	status->SetForegroundColour(wxColour(255, 0, 0));
}
//----------------------------------------------------------------------------
wxAuiManager *E_MainFrame::GetAuiMananger()
{
	if (mAuiManager)
	{
		return mAuiManager;
	}
	return 0;
}
//----------------------------------------------------------------------------