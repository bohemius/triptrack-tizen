#ifndef _FACEBOOK_TOOL_FORM_H_
#define _FACEBOOK_TOOL_FORM_H_

#include <FApp.h>
#include <FBase.h>
#include <FSystem.h>
#include <FUi.h>
#include <FUiIme.h>
#include <FGraphics.h>
#include <FWeb.h>
#include <FNet.h>
#include <gl.h>

struct FacebookAccessToken {
	Tizen::Base::String AccessToken;
	Tizen::Base::String AppId;
	Tizen::Base::String AppSecret;
	long long int ExpiryTime;
};

class FacebookForm: public Tizen::Ui::Controls::Form,
		public Tizen::Ui::Scenes::ISceneEventListener,
		public Tizen::Ui::Controls::IFormBackEventListener,
		public Tizen::Web::Controls::ILoadingListener {

public:
	FacebookForm(void);
	virtual ~FacebookForm(void);
	bool Initialize(void);

	// ISceneEventListener
	virtual void OnSceneActivatedN(
			const Tizen::Ui::Scenes::SceneId& previousSceneId,
			const Tizen::Ui::Scenes::SceneId& currentSceneId,
			Tizen::Base::Collection::IList* pArgs);
	virtual void OnSceneDeactivated(
			const Tizen::Ui::Scenes::SceneId& currentSceneId,
			const Tizen::Ui::Scenes::SceneId& nextSceneId);

	// ILoadingListener
	virtual bool OnHttpAuthenticationRequestedN(const Tizen::Base::String& host,
			const Tizen::Base::String& realm,
			const Tizen::Web::Controls::AuthenticationChallenge& authentication);
	virtual void OnHttpAuthenticationCanceled(void);
	virtual void OnLoadingStarted(void);
	virtual void OnLoadingCanceled(void);
	virtual void OnLoadingErrorOccurred(
			Tizen::Web::Controls::LoadingErrorType error,
			const Tizen::Base::String& reason);
	virtual void OnLoadingCompleted(void);
	virtual void OnPageTitleReceived(const Tizen::Base::String& title);
	virtual bool OnLoadingRequested(const Tizen::Base::String& url,
			Tizen::Web::Controls::WebNavigationType type);
	virtual Tizen::Web::Controls::DecisionPolicy OnWebDataReceived(
			const Tizen::Base::String& mime,
			const Tizen::Net::Http::HttpHeader& header);
	virtual void OnEstimatedProgress(int progress);

private:
	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	result ExtractAccessToken(Tizen::Base::String& url);

	Tizen::Web::Controls::Web* __pWeb;
	Tizen::Base::String __accessUrl;
	FacebookAccessToken __accessToken;
	Tizen::Ui::Scenes::SceneId __previousScene;
};

#endif	//_FACEBOOK_TOOL_FORM_H_
