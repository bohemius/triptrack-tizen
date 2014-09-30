/*
 * StaticMap.h
 *
 *  Created on: Sep 23, 2014
 *      Author: bohemius
 */

#ifndef STATICMAP_H_
#define STATICMAP_H_

#include <FNet.h>
#include <FBase.h>
#include "dao/POI.h"
#include "geo/Tracker.h"

class StaticMap: public Tizen::Net::Http::IHttpTransactionEventListener {
public:
	StaticMap();
	virtual ~StaticMap();
	result Construct(POI* pPoi, long long int& albumId);
	result Construct(Tracker* pTracker);

	// IHttpTransactionEventListener handlers are declared
	virtual void OnTransactionReadyToRead(
			Tizen::Net::Http::HttpSession& httpSession,
			Tizen::Net::Http::HttpTransaction& httpTransaction,
			int availableBodyLen);
	virtual void OnTransactionAborted(
			Tizen::Net::Http::HttpSession& httpSession,
			Tizen::Net::Http::HttpTransaction& httpTransaction, result r);
	virtual void OnTransactionReadyToWrite(
			Tizen::Net::Http::HttpSession& httpSession,
			Tizen::Net::Http::HttpTransaction& httpTransaction,
			int recommendedChunkSize);
	virtual void OnTransactionHeaderCompleted(
			Tizen::Net::Http::HttpSession& httpSession,
			Tizen::Net::Http::HttpTransaction& httpTransaction, int headerLen,
			bool authRequired);
	virtual void OnTransactionCompleted(
			Tizen::Net::Http::HttpSession& httpSession,
			Tizen::Net::Http::HttpTransaction& httpTransaction);
	virtual void OnTransactionCertVerificationRequiredN(
			Tizen::Net::Http::HttpSession& httpSession,
			Tizen::Net::Http::HttpTransaction& httpTransaction,
			Tizen::Base::String* pCert);

	Tizen::Base::ByteBuffer* GetMapBuffer() const {
		return __pMapBuffer;
	}

	void SetMapBuffer(Tizen::Base::ByteBuffer* mapBuffer) {
		__pMapBuffer = mapBuffer;
	}

private:
	result AddToFacebookAlbum(void);
	result AddToFacebook(void);

	Tizen::Base::ByteBuffer* __pMapBuffer;
	long long int __albumId;
	int __state;

	static const int __width = 1280;
	static const int __height = 1024;
};

#endif /* STATICMAP_H_ */
