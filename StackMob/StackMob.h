// Copyright 2011 StackMob, Inc
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#import <Foundation/Foundation.h>
#import "StackMobSession.h"
#import "StackMobRequest.h"
#import "DataProviderProtocol.h"
#import "StackMobConfiguration.h"

typedef enum {
    SMEnvironmentProduction = 0,
    SMEnvironmentDevelopment = 1
} SMEnvironment;


typedef void (^StackMobCallback)(BOOL success, id result);
@class StackMobDataProvider;

@interface StackMob : NSObject <SMRequestDelegate>{
    NSMutableArray *callbacks;
    NSMutableArray *requests;
    StackMobSession *session;
    StackMobRequest *currentRequest;
    BOOL _running;
    id<DataProviderProtocol> _dataProvider;
}

@property (nonatomic, retain) StackMobSession *session;
@property (nonatomic, retain) NSMutableArray *callbacks;
@property (nonatomic, retain) NSMutableArray *requests;

/** 
 * Data bridge which handles network, data mapping, and serialization 
 * features.
 */
@property(nonatomic,retain) id<DataProviderProtocol> dataProvider;

/*
 * Manually configure your session.  Subsequent requests for the StackMob
 * singleton can use [StackMob stackmob].
 */


+ (StackMob *)setApplication:(NSString *)apiKey secret:(NSString *)apiSecret 
                     appName:(NSString *)appName subDomain:(NSString *)subDomain 
              userObjectName:(NSString *)userObjectName apiVersionNumber:(NSNumber *)apiVersion;


/*
 * Manually configure your session.  Subsequent requests for the StackMob
 * singleton can use [StackMob stackmob].
 */

+ (StackMob *)setApplication:(NSString *)apiKey secret:(NSString *)apiSecret 
                     appName:(NSString *)appName subDomain:(NSString *)subDomain 
              userObjectName:(NSString *)userObjectName apiVersionNumber:(NSNumber *)apiVersion
                  dataProvider:(id<DataProviderProtocol>)dataProvider;

/** Sets the stackmob shared object. Set to nil to reset stackmob singleton. */
+ (void) setSharedManager:(StackMob *)stackMob;

/*
 * Returns the pre-configured or auto-configured singleton
 * all instance methods are called on the singleton
 * If this method is called before setApplication:secret:appName:subDomain:userObjectName:apiVersonNumber
 * it will load the app config info from StackMob.plist in the main Bundle
 */
+ (StackMob *)stackmob;

/* 
 * Initializes a user session
 * Make sure to call this in appDidFinishLaunching
 */
- (StackMobRequest *)startSession;

/*
 * Ends a user session
 * Make sure to call this in applicationWillEnterBackground and applicationWillTerminate
 */
- (StackMobRequest *)endSession;

/********************* User based requests *************************/

/*
 * Registers a new userusing the user object name set when initializing StackMobSession
 * @param arguments A dictionary whose keys correspond to object field names on Stackmob Object Model
 */
- (StackMobRequest *)registerWithArguments:(NSDictionary *)arguments andCallback:(StackMobCallback)callback;

/*
 * Logs a user in using the user object name set when initializing StackMobSession
 * @param arguments A dictionary whose keys correspond to object field names on Stackmob Object Model
 */
- (StackMobRequest *)loginWithArguments:(NSDictionary *)arguments andCallback:(StackMobCallback)callback;

/*
 * Logs the current user out
 */
- (StackMobRequest *)logoutWithCallback:(StackMobCallback)callback;

/*
 * Gets a user object using the user object name set when initializing 
 * StackMobSession
 * @param arguments A dictionary whose keys correspond to object field names on Stackmob Object Model
 */
- (StackMobRequest *)getUserInfowithArguments:(NSDictionary *)arguments andCallback:(StackMobCallback)callback;

/********************** Facebook Methods ******************/

/*
 * Authenticates a user in your StackMob app using their Facebook Token.  
 * Assumes the user is already registered
 * @param facebookToken the user's facebook access token
 */
- (StackMobRequest *)loginWithFacebookToken:(NSString *)facebookToken andCallback:(StackMobCallback)callback;

/* 
 * Registers a new user in your StackMob app using their facebook token
 * and a user selected username (you can default it to their Facebook username
 * assuming they have set one)
 * @param facebookToken the user's facebook access token
 */
- (StackMobRequest *)registerWithFacebookToken:(NSString *)facebookToken username:(NSString *)username
                                   andCallback:(StackMobCallback)callback;

/*
 * Links an existing user account to their facebook account.  Assumes the user
 * is currently logged in.
 * @param facebookToken the user's facebook token
 */
- (StackMobRequest *)linkUserWithFacebookToken:(NSString *)facebookToken withCallback:(StackMobCallback)callback;

/*
 * Post a message to facebook for the currently logged in user
 * assumes the user has connected to facebook
 * @param message the message to post
 */
- (StackMobRequest *)postFacebookMessage:(NSString *)message withCallback:(StackMobCallback)callback;

/*
 * Get the user info from facebook for the currently logged in user
 * assumes the user has connected to facebook
 */
- (StackMobRequest *)getFacebookUserInfoWithCallback:(StackMobCallback)callback;

/********************** Twitter methods ***********************/

/*
 * Registers a new user in your app using their twitter account
 * @param token the user's twitter token
 * @param secret the user's twitter secret
 */
- (StackMobRequest *)registerWithTwitterToken:(NSString *)token secret:(NSString *)secret 
                                     username:(NSString *)username andCallback:(StackMobCallback)callback;

/* 
 * Login an existing user via Twitter
 * @param token the user's twitter token
 * @param secret the user's twitter secret
 */
- (StackMobRequest *)loginWithTwitterToken:(NSString *)token secret:(NSString *)secret andCallback:(StackMobCallback)callback;

/*
 * Link an existing user to their twitter account
 * @param token the user's token
 * @param secret the user's secret
 */
- (StackMobRequest *)linkUserWithTwitterToken:(NSString *)token secret:(NSString *)secret andCallback:(StackMobCallback)callback;

/*
 * Send a status update to twitter
 * @param message the status update to send
 */
- (StackMobRequest *)twitterStatusUpdate:(NSString *)message withCallback:(StackMobCallback)callback;

/********************** PUSH Notifications ********************/

/* 
 * Register a User for PUSH notifications
 * @param userId the user's user Id or username
 * @param token the device's PUSH notification token
 * @param arguments a Dictionary 
 */
- (StackMobRequest *)registerForPushWithUser:(NSString *)userId token:(NSString *)token andCallback:(StackMobCallback)callback;

/********************** CRUD Methods **********************/

/* 
 * Get the object with name "path" and arguments dictionary. 
 * @param arguments a dictionary whose keys correspond to object field names on Stackmob Object Model
 */
- (StackMobRequest *)get:(NSString *)path withArguments:(NSDictionary *)arguments andCallback:(StackMobCallback)callback;

/* 
 * Get the object with name "path" with no arguments.  This will return all items of object type. 
 * @param path the name of the object to get in your stackmob app
 */
- (StackMobRequest *)get:(NSString *)path withCallback:(StackMobCallback)callback;

/* 
 * POST the arguments to the given object model with name of "path".  
 * @param path the name of the object in your stackmob app to be created
 * @param arguments a dictionary whose keys correspond to field names of the object in your Stackmob app
 */
- (StackMobRequest *)post:(NSString *)path withArguments:(NSDictionary *)arguments andCallback:(StackMobCallback)callback;

/*
 * POST the arguments for a user. 
 * @param path the name of the object in your stackmob app to be created
 * @param arguments a dictionary whose keys correspond to field names of the object in your Stackmob app
 */
- (StackMobRequest *)post:(NSString *)path forUser:(NSString *)user withArguments:(NSDictionary *)arguments
              andCallback:(StackMobCallback)callback;

/*
 * PUT the arguments to the given object path. 
 * @path the name of the object in your Stackmob app
 * @param arguments a Dictionary of attributes whose  keys correspond to field names of the object in your Stackmob app
 */
- (StackMobRequest *)put:(NSString *)path withArguments:(NSDictionary *)arguments andCallback:(StackMobCallback)callback;

/* 
 * DELETE the object at the given path. 
 * @path the name of the object in your stackmob app
 * @param arguments a Dictonary with one key that corresponds to your object's primary key
 *   the value of which is the item to delete
 */
- (StackMobRequest *)destroy:(NSString *)path withArguments:(NSDictionary *)arguments andCallback:(StackMobCallback)callback;


/********************** NEW CRUD Methods **********************/

/* 
 * Get the object with name "path" and arguments dictionary. 
 * @param object whose property correspond to field names request url
 */
- (StackMobRequest *)get:(NSString *)path withObject:(id)object andCallback:(StackMobCallback)callback;

/* 
 * Get the object with name "path" with no arguments.  This will return all items of object type. 
 * @param object the name of the object to get in your stackmob app
 */
- (StackMobRequest *)get:(NSString *)path withCallback:(StackMobCallback)callback;

/* 
 * POST the arguments to the given object model with name of "path".
 * @param path the name of the object in your stackmob app to be created
 * @param object whose property correspond to field names of the object in your Stackmob app
 */
- (StackMobRequest *)post:(NSString *)path withObject:(id)object andCallback:(StackMobCallback)callback;

/*
 * POST the data for a user.  
 * @param path the name of the object in your stackmob app to be created
 * @param object whose property correspond to field names of the object in your Stackmob app
 */
- (StackMobRequest *)post:(NSString *)path forUser:(NSString *)user withObject:(id)object
              andCallback:(StackMobCallback)callback;

/*
 * PUT the data to the given object path. 
 * @path the name of the object in your Stackmob app
 * @param object whose property correspond to field names of the object in your Stackmob app
 */
- (StackMobRequest *)put:(NSString *)path withObject:(id)object andCallback:(StackMobCallback)callback;

/* 
 * DELETE the object at the given path. 
 * @path the name of the object in your stackmob app
 * @param object a with a primary key which will be used in request to delete object.
 *   the value of which is the item to delete
 */
- (StackMobRequest *)destroy:(NSString *)path withObject:(id)object andCallback:(StackMobCallback)callback;

/**************** Heroku Methods *****************/

/*
 * Perform a GET request on a custom heroku action.
 * @param path should be the full path without the leading / to your action
 */
- (StackMobRequest *)herokuGet:(NSString *)path withCallback:(StackMobCallback)callback;

/*
 * Perform a GET request on a custom heroku action.
 * @param path should be the full path without the leading / to your action
 * @param arguments a dictionary to be converted to query params
 */
- (StackMobRequest *)herokuGet:(NSString *)path withArguments:(NSDictionary *)arguments andCallback:(StackMobCallback)callback;

/*
 * POST the arguments for a heroku action on heroku
 * @param path the name of the object in your stackmob app to be created (without 'heroku/proxy')
 * @param arguments a dictionary whose keys correspond to field names of the object in your Stackmob app
 */
- (StackMobRequest *)herokuPost:(NSString *)path withArguments:(NSDictionary *)arguments andCallback:(StackMobCallback)callback;

/*
 * PUT the arguments for a heroku action on heroku
 * @param path the name of the object in your stackmob app to be created (without 'heroku/proxy')
 * @param arguments a dictionary whose keys correspond to field names of the object in your Stackmob app
 */
- (StackMobRequest *)herokuPut:(NSString *)path withArguments:(NSDictionary *)arguments andCallback:(StackMobCallback)callback;

/*
 * DELETE the object at specified path
 * @param path should be the full path without the leading / to your action
 */
- (StackMobRequest *)herokuDelete:(NSString *)path andCallback:(StackMobCallback)callback;

@end
