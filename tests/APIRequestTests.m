// Copyright 2011 StackMob, Inc
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#import "APIRequestTests.h"

StackMobSession *mySession = nil;

@implementation APIRequestTests

- (void) setUp
{
    [super setUp];
	NSLog(@"In setup");
	if (!mySession)
	{
		NSLog(@"Created new session");
	}
}

- (void) tearDown
{
	NSLog(@"In teardown");
	mySession = nil;
    [super tearDown];
}

- (void) testGet {
    StackMobQuery *q = [StackMobQuery query];
    [q field:@"username" mustEqualValue:@"ty"];
    [q field:@"createddate" mustBeGreaterThanOrEqualToValue:[NSNumber numberWithInt:2]];
    
	
	StackMobRequest *request = [StackMobRequest requestForMethod:@"user" 
                                                       withQuery:q
												  withHttpVerb:GET];
	[request sendRequest];
	//we need to loop until the request comes back, its just a test its OK
    [StackMobTestUtils runRunLoop:[NSRunLoop currentRunLoop] untilRequestFinished:request];
	    
    STAssertTrue([[request result] isKindOfClass:[NSArray class]], @"Did not get a valid GET result");
	request = nil;

}

- (void) testPost {
	NSLog(@"IN TEST POST");
    NSMutableDictionary* userArgs = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
										@"Ty", @"firstname",
										@"Amell", @"lastname",
										@"ty@stackmob.com", @"email",
										nil];
	
    StackMobRequest *request = [[StackMob stackmob] post:@"user" withArguments:userArgs andCallback:^(BOOL success, id result){
        if(success){
            NSLog(@"testPost result was: %@", result);
            NSDictionary *info = (NSDictionary *)result;
            NSString *userId = [info objectForKey:@"username"];
            STAssertNotNil(userId, @"Returned value for POST is not correct");
        }
        else{
            STFail(@"creating a user failed");
        }
    }];

	//we need to loop until the request comes back, its just a test its OK
    NSDictionary * result = [StackMobTestUtils runDefaultRunLoopAndGetDictionaryResultFromRequest:request];
    NSLog(@"result: %@", result);
	NSString *userId = [result objectForKey:@"username"];
	STAssertNotNil(userId, @"Returned value for POST is not correct");
	request = nil;
	[userArgs release];
}


- (void)testBulkPost {
    NSMutableDictionary* userArgs1 = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
                                     @"Ty", @"firstname",
                                     @"Amell", @"lastname",
                                     @"ty@stackmob.com", @"email",
                                     nil];
    NSMutableDictionary* userArgs2 = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
                                      @"Jordan", @"firstname",
                                      @"West", @"lastname",
                                      @"jordan@stackmob.com", @"email",
                                      nil];

    StackMobRequest *r = [[StackMob stackmob] post:@"user" withBulkArguments:[NSArray arrayWithObjects:userArgs1, userArgs2, nil] andCallback:^(BOOL success, id result) {}];
    
    [self assertNotNSError:[StackMobTestUtils runDefaultRunLoopAndGetDictionaryResultFromRequest:r]];                          
}
- (void) testDoubleFieldSet {
    NSMutableDictionary * dict = [NSMutableDictionary dictionary];
    NSString * key = @"abc";
    NSNumber * value = [NSNumber numberWithDouble:2.0];
    [dict setValue:(NSNumber *)value forKey:key];
 
    StackMobRequest * request = [[StackMob stackmob] post:@"test" withArguments:dict andCallback:^(BOOL success, id result) {
        if (success) {
            SMLog(@"response: %@", result);
        } else {
            STFail(@"failure with result %@", result);
        }
    }];
    NSDictionary * result = [StackMobTestUtils runDefaultRunLoopAndGetDictionaryResultFromRequest:request];
    NSLog(@"result: %@", result);
}

- (void) testDoubleJSONEncoding {
    static NSString * key = @"doubleKey";
    NSNumber * doubleNumber = [NSNumber numberWithDouble:22];
    NSMutableDictionary * dict = [NSMutableDictionary dictionary];
    [dict setObject:doubleNumber forKey:key];
    
    NSError * error = nil;
    NSData * jsonData = [StackMobRequest JsonifyNSDictionary:dict withErrorOutput:&error];
    STAssertNil(error, @"json encoding had error: %@", error);
    
    NSString * jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    id decodedObject = [jsonString objectFromJSONString];
    if(![decodedObject isKindOfClass:[NSDictionary class]]) {
        STFail(@"JSON decoded object is not a dictionary or subclass");
    }
    
    NSDictionary * decodedDict = (NSDictionary *)decodedObject;
    id decodedObjectValue = [decodedDict objectForKey:key];
    if(![decodedObjectValue isKindOfClass:[NSNumber class]]) {
        STFail(@"object taken from dictionary was not an NSNumber");
    }
    
    NSNumber * decodedNumberValue = (NSNumber *)decodedObjectValue;
    STAssertTrue([decodedNumberValue isEqual:doubleNumber], @"%@ was not equal to the value for %@ taken from the decoded dictionary", [decodedDict objectForKey:key], key);
}

- (void) testURLGeneration {

	StackMobRequest *request = [StackMobRequest requestForMethod: @"user"];
	NSURL *testURL = [NSURL URLWithString: @"http://stackmob.stackmob.com/api/0/iossdktest/user"];
    NSLog(@"expected %@", [testURL absoluteString]),
    NSLog(@"actual %@", [request.url absoluteString]);
	STAssertTrue([[testURL absoluteString] isEqualToString: 
				  [request.url absoluteString]], @"User get URLs do not match" );
	testURL = nil;
	request = nil;
	
}

- (void) testSecureURLGeneration {
    StackMobRequest *request = [StackMobRequest requestForMethod:@"user"];
    request.isSecure = YES;
    NSURL *expectedURL = [NSURL URLWithString:@"https://stackmob.stackmob.com/api/0/iossdktest/user"];
    STAssertTrue([[expectedURL absoluteString] isEqualToString:[request.url absoluteString]], @"%@ Does Not Match Expected Secure URL", [request.url absoluteString]);
    expectedURL = nil;
    request = nil;
}

- (void) testAPIList {
	
	StackMobRequest *request = [StackMobRequest requestForMethod: @"listapi"];
	NSLog(@"Calling sendSynchronousRequest");
    NSError *error = nil;
	NSDictionary *result = [request sendSynchronousRequestProvidingError:&error];
    STAssertNotNil([result objectForKey:@"user"], @"No User Object in List API, Fail");
	request = nil;		
}

- (void) testRequestsThatDefaultToSecure {
    StackMobRequest *r;
    
    r = [[StackMob stackmob] loginWithArguments:[NSDictionary dictionary] andCallback:emptyCallback];
    [self assertNotNSError:r];
    STAssertTrue(r.isSecure, @"Login Request Should Default to SSL");
                    
    r = [[StackMob stackmob] loginWithFacebookToken:@"WHOCARES" andCallback:emptyCallback];
    [self assertNotNSError:r];
    STAssertTrue(r.isSecure, @"Login w/ Facebook Request Should Default to SSL");
    
    r = [[StackMob stackmob] loginWithTwitterToken:@"WHOCARES" secret:@"WHOCARES2" andCallback:emptyCallback];
    [self assertNotNSError:r];
    STAssertTrue(r.isSecure, @"Login w/ Twitter Request Should Default to SSL");
    
    r = [[StackMob stackmob] linkUserWithFacebookToken:@"ASD" withCallback:emptyCallback];
    [self assertNotNSError:r];
    STAssertTrue(r.isSecure, @"Link With Facebook Token Should Default to SSL");
    
    r = [[StackMob stackmob] linkUserWithTwitterToken:@"WHOCARES" secret:@"WHOCARES" andCallback:emptyCallback];
    [self assertNotNSError:r];
    STAssertTrue(r.isSecure, @"Link With Twitter Token Should Default to SSL");

    r = [[StackMob stackmob] registerWithArguments:[NSDictionary dictionary] andCallback:emptyCallback];
    [self assertNotNSError:r];
    STAssertTrue(r.isSecure, @"Register User Should Default to SSL");
    
    r = [[StackMob stackmob] registerWithFacebookToken:@"TOKEN" username:@"UNAME" andCallback:emptyCallback];
    [self assertNotNSError:r];
    STAssertTrue(r.isSecure, @"Register With Facebook Token Should Default to SSL");
    
    r = [[StackMob stackmob] registerWithTwitterToken:@"TOKEN" secret:@"SECRET" username:@"UNAME" andCallback:emptyCallback];
    [self assertNotNSError:r];
    STAssertTrue(r.isSecure, @"Register With Twitter Token Should Defult to SSL");
        
}


@end
