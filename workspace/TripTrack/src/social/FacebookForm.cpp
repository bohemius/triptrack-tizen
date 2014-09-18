#include "social/FacebookForm.h"
#include "dao/StorageManager.h"
#include "AppResourceId.h"

using namespace Tizen::Base;
using namespace Tizen::Base::Utility;
using namespace Tizen::Base::Collection;
using namespace Tizen::App;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Web::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::Net;
using namespace Tizen::Net::Http;

FacebookForm::FacebookForm(void) :
		__pWeb(null) {
}

FacebookForm::~FacebookForm(void) {
}

bool FacebookForm::Initialize(void) {
	result r = Construct(FORM_STYLE_NORMAL);
	TryReturn(r == E_SUCCESS, false, "Failed to construct form");

	return true;
}
result FacebookForm::OnInitializing(void) {
	result r = E_SUCCESS;

	SetFormBackEventListener(this);


	__accessToken = StorageManager::getInstance()->GetFacebookCredentials();

	SetBackgroundColor(Color::GetColor(COLOR_ID_CYAN));
	__accessUrl = L"https://www.facebook.com/dialog/oauth?client_id=";
	__accessUrl.Append(__accessToken.AppId);
	__accessUrl.Append(
			"&redirect_uri=http://www.facebook.com/connect/login_success.html&response_type=token&display=touch&scope=user_photos, publish_actions");

	__pWeb = new (std::nothrow) Web();

	r = __pWeb->Construct(GetBounds());
	//r = __pWeb->Construct(Rectangle(10,10,200,400));
	if (r != E_SUCCESS)
		AppLogException("Error constructing web view: ", GetErrorMessage(r));

	r = AddControl(__pWeb);
	__pWeb->SetShowState(false);
	__pWeb->SetLoadingListener(this);

	if (r != E_SUCCESS)
		AppLogException( "Error adding web view to form: ", GetErrorMessage(r));

	return r;
}

result FacebookForm::OnTerminating(void) {
	result r = E_SUCCESS;

	// TODO: Add your termination code here

	return r;
}

bool FacebookForm::OnHttpAuthenticationRequestedN(
		const Tizen::Base::String& host, const Tizen::Base::String& realm,
		const Tizen::Web::Controls::AuthenticationChallenge& authentication) {
}

void FacebookForm::OnHttpAuthenticationCanceled(void) {
}

void FacebookForm::OnLoadingStarted(void) {
	result r = E_SUCCESS;

	AppLog("Loading started, URL is: %ls", __pWeb->GetUrl().GetPointer());

	//__pWeb->SetShowState(false);
}

void FacebookForm::OnLoadingCanceled(void) {
}

void FacebookForm::OnLoadingErrorOccurred(
		Tizen::Web::Controls::LoadingErrorType error,
		const Tizen::Base::String& reason) {

	MessageBox msgBox;
	int modalresult = 0;
	String errReason;

	switch (error) {
	case WEB_REQUEST_TIMEOUT: {
		errReason = Tizen::Base::String(L"Request Timeout");
	}
		break;

	case WEB_NO_CONNECTION: {
		errReason = Tizen::Base::String(L"Network is not in service");
	}
		break;

	case WEB_MIME_NOT_SUPPORTED: {
		errReason = Tizen::Base::String(L"The content type is not supported");
	}
		break;

	case WEB_BAD_URL: {
		errReason = Tizen::Base::String(L"The url is invalid");
	}
		break;

	case WEB_HTTP_RESPONSE: {
		errReason = Tizen::Base::String(L"HTTP Response");
	}
		break;

	case WEB_OUT_OF_MEMORY: {
		errReason = Tizen::Base::String(L"Page Too Large");
	}
		break;

	case WEB_REQUEST_MAX_EXCEEDED: {
		errReason = Tizen::Base::String(L"Request Max Exceeded");
	}
		break;

	case WEB_ERROR_UNKNOWN:
	default: {
		errReason = Tizen::Base::String(L"An Unknown Error");
	}
		break;
	}

	msgBox.Construct(L"LOADING ERROR TYPE", errReason, MSGBOX_STYLE_NONE, 3000);
	msgBox.ShowAndWait(modalresult);
}

void FacebookForm::OnLoadingCompleted(void) {
	result r = E_SUCCESS;

	String url = String(__pWeb->GetUrl());
	int redirectIndex;

	AppLog("Loading completed, URL is: %ls", url.GetPointer());

	r = url.IndexOf(L"redirect_uri", 0, redirectIndex);
	if (r != E_SUCCESS) {
		r = ExtractAccessToken(url);

		if (__accessToken.ExpiryTime > 0) {
			__pWeb->SetShowState(false);
			AppLog(
					"Got access token: %ls", __accessToken.AccessToken.GetPointer());
			AppLog("Access token expiry: %lld ", __accessToken.ExpiryTime);
			OnFormBackRequested(*this);
		}
	} else {
		WebSetting setting;

		setting.SetInputStyle(INPUT_STYLE_OVERLAY);
		r = __pWeb->SetSetting(setting);
		if (r != E_SUCCESS)
			AppLogException(
					"Error applying settings to web view: ", GetErrorMessage(r));

		__pWeb->SetShowState(true);
		__pWeb->SetFocus();
		RequestRedraw(true);
	}
}

void FacebookForm::OnEstimatedProgress(int progress) {
}

void FacebookForm::OnPageTitleReceived(const Tizen::Base::String& title) {
}

bool FacebookForm::OnLoadingRequested(const Tizen::Base::String& url,
		Tizen::Web::Controls::WebNavigationType type) {
	return false;
}

Tizen::Web::Controls::DecisionPolicy FacebookForm::OnWebDataReceived(
		const Tizen::Base::String& mime,
		const Tizen::Net::Http::HttpHeader& header) {
	return WEB_DECISION_CONTINUE;
}

void FacebookForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source) {
	/*Invalidate(true);
	 SetShowState(false);

	 RemoveAllControls();*/
	SceneManager* pSceneMngr = SceneManager::GetInstance();

	pSceneMngr->GoBackward(BackwardSceneTransition(__previousScene),null);
}

result FacebookForm::ExtractAccessToken(String& url) {
	result r = E_SUCCESS;

	int paramStartIndex = -1;
	String parameters;

	r = url.IndexOf('#', 0, paramStartIndex);
	if (r == E_OUT_OF_RANGE) {
		AppLogException("Search index out of range: ", GetErrorMessage(r));
		return r;
	} else if (r == E_OBJ_NOT_FOUND) {
		AppLogException("No URL parameters found: ", GetErrorMessage(r));
		return r;
	} else {
		r = url.SubString(paramStartIndex + 1, parameters);
	}

	String delim = L"&";
	StringTokenizer paramTokenizer(parameters, delim);

	LinkedListT<String>* pParamValuePairList = new LinkedListT<String>();

	while (paramTokenizer.HasMoreTokens()) {
		String token;
		r = paramTokenizer.GetNextToken(token);
		if (r != E_SUCCESS)
			AppLogException(
					"Error getting param value pair token from the URL", GetErrorMessage(r));
		else
			pParamValuePairList->Add(token);
	}

	IEnumeratorT<String>* pEnum = pParamValuePairList->GetEnumeratorN();

	while (pEnum->MoveNext() == E_SUCCESS) {
		String pair, var, value;
		int equalSignIndex;

		r = pEnum->GetCurrent(pair);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting variable value pair from URL parameters.", GetErrorMessage(r));
		} else {
			r = pair.IndexOf('=', 0, equalSignIndex);
			if (r != E_SUCCESS)
				AppLogException(
						"Error getting = sign index from value pair.", GetErrorMessage(r));
			else {
				pair.SubString(0, equalSignIndex, var);
				pair.SubString(equalSignIndex + 1, value);
				AppLog(
						"Parameter value pair: %ls=%ls", var.GetPointer(), value.GetPointer());
				if (var == "access_token")
					__accessToken.AccessToken = String(value);

				if (var == "expires_in")
					LongLong::Parse(value, __accessToken.ExpiryTime);
			}
		}
	}

	pParamValuePairList->RemoveAll();
	delete pParamValuePairList;
	delete pEnum;
}

void FacebookForm::OnTransactionCompleted(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction) {
	result r = E_SUCCESS;

	if (httpTransaction.GetResponse()->GetHttpStatusCode() == HTTP_STATUS_OK) {
		ByteBuffer* pBuf = httpTransaction.GetResponse()->ReadBodyN();

		int count = 0;
		String body = L"";
		pBuf->Reset();

		AppLog("Reading http response body");

		while (pBuf->HasRemaining()) {
			byte val;
			r = pBuf->GetByte(val);

			if (r != E_SUCCESS)
				AppLogException(
						"Error reading byte number %d", count, GetErrorMessage(r));
			else {
				body.Append((char) val);
				count++;
			}
		}

		AppLog("Read %d bytes.\n", body.GetLength());
		AppLog("%ls", body.GetPointer());
	} else
		AppLogException(
				"Error during transaction: %d", httpTransaction.GetResponse()->GetHttpStatusCode());
}

void FacebookForm::OnHttpUploadInProgress(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction,
		long long currentLength, long long totalLength) {
}

void FacebookForm::OnTransactionReadyToRead(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction,
		int availableBodyLen) {
}

void FacebookForm::OnTransactionAborted(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction, result r) {
}

void FacebookForm::OnTransactionReadyToWrite(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction,
		int recommendedChunkSize) {
}

void FacebookForm::OnTransactionHeaderCompleted(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction, int headerLen,
		bool authRequired) {
}

void FacebookForm::OnTransactionCertVerificationRequiredN(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction,
		Tizen::Base::String* pCert) {
}

void FacebookForm::OnHttpDownloadInProgress(HttpSession& httpSession,
		HttpTransaction& httpTransaction, long long currentLength,
		long long totalLength) {
}

result FacebookForm::CreateFacebookAlbum(void) {
	result r = E_SUCCESS;

	HttpSession* pSession = null;
	HttpTransaction* pTransaction = null;
	HttpRequest* pRequest = null;
	HttpMultipartEntity* pMultipartEntity = null;

	//HttpMultipartEntity* pMultipartEntity = null;
	String hostAddr(L"https://graph.facebook.com");

	String reqUri = String(hostAddr);
	reqUri.Append(L"/v2.1/me/albums?");
	reqUri.Append(L"access_token=");
	reqUri.Append(__accessToken.AccessToken);
	//reqUri.Append(L"&name=%7Btest album%7D");
	//reqUri.Append(L"&message=%7Btest album description%7D");

	pMultipartEntity = new HttpMultipartEntity();
	pMultipartEntity->Construct();
	pMultipartEntity->AddStringPart(L"name", L"test");
	pMultipartEntity->AddStringPart(L"message", L"test msg");

	AppLog("Creating http post request %ls", reqUri.GetPointer());
	// Creates an HTTP session.
	pSession = new HttpSession();
	r = pSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, hostAddr, null);
	if (r != E_SUCCESS) {
		AppLogException("Error constructing http session", GetErrorMessage(r));
		return r;
	}

	pTransaction = pSession->OpenTransactionN();
	r = pTransaction->AddHttpTransactionListener(*this);
	r = pTransaction->SetHttpProgressListener(*this);

	pRequest = pTransaction->GetRequest();
	pRequest->SetMethod(NET_HTTP_METHOD_POST);
	pRequest->SetEntity(*pMultipartEntity);

	r = pRequest->SetUri(reqUri);
	if (r != E_SUCCESS) {
		AppLogException("Error http request URI", GetErrorMessage(r));
		return r;
	}

	pTransaction->SetUserObject(pMultipartEntity);
	r = pTransaction->Submit();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error submitting http transaction", GetErrorMessage(r));
		return r;
	}

	return r;
}

void FacebookForm::OnSceneActivatedN(
		const Tizen::Ui::Scenes::SceneId& previousSceneId,
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		Tizen::Base::Collection::IList* pArgs) {
	__previousScene = previousSceneId;

	AppLog("Activated Facebook oauth form, loading oauth URL");
	__pWeb->LoadUrl(__accessUrl);
}

void FacebookForm::OnSceneDeactivated(
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		const Tizen::Ui::Scenes::SceneId& nextSceneId) {
	StorageManager::getInstance()->UpdateFacebookCredentials(__accessToken);
	AppLog("Deactivated Facebook oauth form");
}

