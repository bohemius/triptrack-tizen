/*
 * StaticMap.cpp
 *
 *  Created on: Sep 23, 2014
 *      Author: bohemius
 */

#include <FGraphics.h>
#include <FUi.h>
#include "geo/StaticMap.h"
#include "geo/Tracker.h"
#include "util/GraphicsUtils.h"
#include "dao/StorageManager.h"
#include "dao/TTLocation.h"

using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Graphics;
using namespace Tizen::Net::Http;
using namespace Tizen::Ui::Controls;

StaticMap::StaticMap() :
		__pMapBuffer(null), __albumId(-1), __state(-1) {
	// TODO Auto-generated constructor stub

}

StaticMap::~StaticMap() {
	// TODO Auto-generated destructor stub
}

result StaticMap::Construct(POI* pPoi, long long int& albumId) {
	result r = E_SUCCESS;

	__albumId = albumId;
	String hostAddr = String(L"http://image.maps.cit.api.here.com");
	String mapUri = String(hostAddr);

	mapUri.Append(
			L"/mia/1.6/route?app_id=SkH6ws8o9MTThvyKJLJN&app_code=14LI-CnPtBQEtVEKofyY9w&poitxs=25&poifc=&w=");
	mapUri.Append(__width);
	mapUri.Append(L"&h=");
	mapUri.Append(__height);
	mapUri.Append("&poi=");

	if (pPoi != null) {
		mapUri.Append(pPoi->GetLocation()->GetLatitude());
		mapUri.Append(',');
		mapUri.Append(pPoi->GetLocation()->GetLongitude());

		LinkedListT<Tracker*>* trackList=StorageManager::getInstance()->GetTracks(pPoi);
		if (trackList->GetCount() ==1 ) {
			mapUri.Append("&r=");
			Tracker* pTracker;
			trackList->GetAt(0, pTracker);
			IEnumeratorT<TTLocation*>* pEnum=pTracker->GetTrack()->GetEnumeratorN();
			while (pEnum->MoveNext() == E_SUCCESS) {
				TTLocation* pLoc;
				pEnum->GetCurrent(pLoc);
				mapUri.Append(pLoc->getLatitude());
				mapUri.Append(',');
				mapUri.Append(pLoc->getLongitude());
				mapUri.Append(',');
			}
			mapUri.Remove(mapUri.GetLength()-1,1);
		}

		HttpSession* pSession = null;
		HttpTransaction* pTransaction = null;
		HttpRequest* pRequest = null;
		HttpHeader* pHeader = null;

		AppLog(
				"Creating http post request for static map %ls", mapUri.GetPointer());
		// Creates an HTTP session.
		pSession = new HttpSession();
		r = pSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, hostAddr,
				null);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error constructing http session for static map", GetErrorMessage(r));
			return r;
		}

		pTransaction = pSession->OpenTransactionN();
		r = pTransaction->AddHttpTransactionListener(*this);

		pRequest = pTransaction->GetRequest();
		pRequest->SetMethod(NET_HTTP_METHOD_GET);

		r = pRequest->SetUri(mapUri);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error http request URI for static map", GetErrorMessage(r));
			return r;
		}

		pHeader = pRequest->GetHeader();
		r = pHeader->AddField("Accept", "image/jpeg");
		if (r != E_SUCCESS) {
			AppLogException(
					"Error adding field to header ", GetErrorMessage(r));
			return r;
		}

		r = pTransaction->Submit();
		if (r != E_SUCCESS) {
			AppLogException(
					"Error submitting http transaction", GetErrorMessage(r));
			return r;
		}
	} else {
		AppLogException("POI is null cannot construct map.");
		r = E_INVALID_DATA;
	}

	return r;

}

void StaticMap::OnTransactionReadyToRead(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction,
		int availableBodyLen) {
}

void StaticMap::OnTransactionAborted(Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction, result r) {
}

void StaticMap::OnTransactionReadyToWrite(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction,
		int recommendedChunkSize) {
}

void StaticMap::OnTransactionHeaderCompleted(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction, int headerLen,
		bool authRequired) {
}

void StaticMap::OnTransactionCompleted(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction) {
	result r=E_SUCCESS;
	HttpResponse* pHttpResponse = null;

	pHttpResponse = httpTransaction.GetResponse();

	if (pHttpResponse->GetHttpStatusCode() == HTTP_STATUS_OK && __state < 0) {
		__pMapBuffer = pHttpResponse->ReadBodyN();
		AppLog(
				"Successfully got static map image buffer with size %d", __pMapBuffer->GetLimit());
		__state = 1;
		r = AddToFacebookAlbum();
		if (r != E_SUCCESS) {
			AppLogException(
					"Error adding static map to facebook album", GetErrorMessage(r));
			return;
		}

	} else if (pHttpResponse->GetHttpStatusCode() == HTTP_STATUS_OK
			&& __state > 0) {
		AppLog(
				"Successfully posted static map image to facebook album with id %lld", __albumId);
	} else {
		AppLogException(
				"Error completing static map http transaction: %d", pHttpResponse->GetHttpStatusCode());
		AppLogException("Raw header: %ls", pHttpResponse->GetHeader()->GetRawHeaderN()->GetPointer());
	}
}

void StaticMap::OnTransactionCertVerificationRequiredN(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction,
		Tizen::Base::String* pCert) {
}

result StaticMap::AddToFacebookAlbum(void) {
	result r = E_SUCCESS;

	HttpSession* pSession = null;
	HttpTransaction* pTransaction = null;
	HttpRequest* pRequest = null;
	HttpMultipartEntity* pMultipartEntity = null;

	String hostAddr(L"https://graph.facebook.com");

	String reqUri = String(hostAddr);
	reqUri.Append(L"/v2.1/");
	reqUri.Append(__albumId);
	reqUri.Append("/photos?");
	reqUri.Append(L"access_token=");
	reqUri.Append(
			StorageManager::getInstance()->GetFacebookCredentials().AccessToken);

	AppLog("Creating http post request %ls", reqUri.GetPointer());
	// Creates an HTTP session.
	pSession = new HttpSession();
	r = pSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, hostAddr, null);
	if (r != E_SUCCESS) {
		AppLogException( "Error constructing http session", GetErrorMessage(r));
		return r;
	}

	pTransaction = pSession->OpenTransactionN();
	r = pTransaction->AddHttpTransactionListener(*this);

	pRequest = pTransaction->GetRequest();
	pRequest->SetMethod(NET_HTTP_METHOD_POST);

	r = pRequest->SetUri(reqUri);
	if (r != E_SUCCESS) {
		AppLogException("Error http request URI", GetErrorMessage(r));
		return r;
	}

	AppLog( "Adding static map to post request");
	pMultipartEntity = new HttpMultipartEntity();
	pMultipartEntity->Construct();

	pMultipartEntity->AddFilePartByBuffer(L"source", L"map", *__pMapBuffer);

	pRequest->SetEntity(*pMultipartEntity);
	pTransaction->SetUserObject(pMultipartEntity);
	r = pTransaction->Submit();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error submitting http transaction", GetErrorMessage(r));
		return r;
	}

	return r;
}

