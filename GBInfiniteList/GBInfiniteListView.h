//
//  GBInfiniteListView.h
//  GBInfiniteList
//
//  Created by Luka Mirosevic on 30/04/2013.
//  Copyright (c) 2013 Goonbee. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol GBInfiniteListDataSource;
@protocol GBInfiniteListDelegate;

@interface GBInfiniteListView : UIView <UIScrollViewDelegate>

//Set delegate before setting dataSource
@property (weak, nonatomic) id<GBInfiniteListDataSource>        dataSource;
@property (weak, nonatomic) id<GBInfiniteListDelegate>          delegate;

//Returns 0 if the Geometry dataSource methods haven't been called yet
@property (assign, nonatomic, readonly) CGFloat                 requiredViewWidth;

#pragma mark - Designated initialiser

//Designated initialiser. If the frame changes, the actual list(which is a subview of this object) is just centered. You have to call reset and supply new paddings, column count, etc. to change the geometric properties of the list.
-(id)initWithFrame:(CGRect)frame;

//foo maybe add initwithcoder for use with IB

#pragma mark - Data dance

//Asks for the views of the visible items again of the dataSource
-(void)reloadVisibleItems;

//reset (removes everything, cleans up memory and scrolls to top with no animation)
-(void)reset;

//Hides the loading thing and requests the next item
-(void)didFinishLoadingMoreItems;

//Lets you recycle views which have gone off screen rather than creating and destroying them all the time
-(UIView *)dequeueReusableViewWithIdentifier:(NSString *)viewIdentifier;

#pragma mark - Scrolling & Co.

//Just scrolls to top
-(void)scrollToTopAnimated:(BOOL)shouldAnimate;

//Pass in a location to which the top edge should scroll (this is thresholded so you don't scroll too far)
-(void)scrollToPosition:(CGFloat)yPosition animated:(BOOL)shouldAnimate;

//Lets you check if item is on screen?
-(BOOL)isItemOnScreen:(NSUInteger)itemIdentifier;

//Lets you get an array of all the currently displayed items.
-(NSArray *)itemsCurrentlyOnScreen;

@end

@protocol GBInfiniteListDataSource <NSObject>

#pragma mark - Geometry
//These are called after init, or after reset... only once. It's recommended you stick to integral values.

@required

//How many columns
-(NSUInteger)numberOfColumnsInInfiniteListView:(GBInfiniteListView *)infiniteListView;

@optional

//The outermost padding of the list. This encapsulates the header, loading and noItems views as well (by default). This one is called first of all the geometry methods. Defaults to 0 for all edges
-(UIEdgeInsets)outerPaddingInInfiniteListView:(GBInfiniteListView *)infiniteListView;

//These are collapsible. Defaults to 0
-(CGFloat)verticalItemMarginInInfiniteListView:(GBInfiniteListView *)infiniteListView;

//This is collapsible Defaults to 0
-(CGFloat)horizontalColumnMarginInInfiniteListView:(GBInfiniteListView *)infiniteListView;

#pragma mark - Data dance

@required

//Called before requesting each view. If you return NO, you are asked if there are more items available, if you return YES, you are asked for that item's view
-(BOOL)isViewForItem:(NSUInteger)itemIdentifier currentlyAvailableInInfiniteListView:(GBInfiniteListView *)infiniteListView;

//Return a view for the particular item. The width must be correct. You can ask the infiniteListView for the requiredViewWidth. If you need to resize your view before they are displayed by the infiniteListView, you must do it yourself first, otherwise a "ViewWidthMismatchException" is thrown
-(UIView *)viewForItem:(NSUInteger)itemIdentifier inInfiniteListView:(GBInfiniteListView *)infiniteListView;

//are there more items? it calls this when you say no to isViewForItem:currentlyAvalibaleInInfiniteListView:; if you return YES: it asks you whether the loading indicator should be shown, the startedLoading delegate method is called, and the loadMore dataSource method is called. When you are done loading, you should call "didFinishLoadingMoreItems" on the infiniteListView; if you return NO: you can't scroll any more, and the delegate noMoreItems methods is called.
-(BOOL)canLoadMoreItemsInInfiniteListView:(GBInfiniteListView *)infiniteListView;

//Lets you know that you should start loading more items. When you are done loading, call the didFinishLoadingMoreItems method on the infiniteListView. Don't load synchronously inside this method, or you'll block the UI!
-(void)startLoadingMoreItemsInInfiniteListView:(GBInfiniteListView *)infiniteListView;

@optional

//Lets you know that a particular view was recycled and it's not showing the previous item any more... in case you were loading stuff async and just put a placeholder image for a particular item. After this point you should release the pointer you had to the view which you were hoping to update once your async load finished. It's a good idea to cache the result once your load finished in case the user scrolls back up, but don't update the view until asked to do so!
-(void)infiniteListView:(GBInfiniteListView *)infiniteListView didRecycleView:(UIView *)view lastUsedByItem:(NSUInteger)itemIdentifier;

#pragma mark - Subviews (Header, No items & Loading)
//Height is kept as is, width is stretched according to padding preference. You can return nil if you don't want anything, or just don't implement it

@optional

//Lets you add a header view, e.g. a search field at the top
-(UIView *)headerViewInInfiniteListView:(GBInfiniteListView *)infiniteListView;
//You can choose whether it should take into account top, left and right outer-padding or not. Default is YES
-(BOOL)shouldRespectPaddingForHeaderViewInInfiniteListView:(GBInfiniteListView *)infiniteListView;
//Lets you choose a margin for between the headerView and actual list. Is 0 by default on all sides. Collapses with verticalItemMargin
-(CGFloat)marginForHeaderViewInInfiniteListView:(GBInfiniteListView *)infiniteListView;

//Lets you show a view when there are no items to show. e.g. if you apply a search filter and there's no results, or if your service sucks and there's nothing to show
-(UIView *)noItemsViewInInfiniteListView:(GBInfiniteListView *)infiniteListView;
//You can choose whether it should take into account top, left and right outer-padding or not. Default is YES
-(BOOL)shouldRespectPaddingForNoItemsViewInInfiniteListView:(GBInfiniteListView *)infiniteListView;

//return YES if you want a loading indicator. Defaults to YES.
-(BOOL)shouldShowLoadingIndicatorInInfiniteListView:(GBInfiniteListView *)infiniteListView;
//Lets you specify a custom loading view, if you return nil or don't implement, a standard spinner is used.
-(UIView *)loadingViewInInfiniteListView:(GBInfiniteListView *)infiniteListView;
//You can choose whether it should take into account bottom, left and right outer-padding or not. Default is YES
-(BOOL)shouldRespectPaddingForLoadingViewInInfiniteListView:(GBInfiniteListView *)infiniteListView;
//Lets you choose a margin for between the loadingView and the list. It's 0 by default on all sides. Collapses with verticalItemMargin
-(CGFloat)marginForLoadingViewInInfiniteListView:(GBInfiniteListView *)infiniteListView;

@end

@protocol GBInfiniteListDelegate <NSObject>

@optional

//Sent when the user taps on a particular view... unless the view handles the touches itself
-(void)infiniteListView:(GBInfiniteListView *)infiniteListView didTapOnView:(UIView *)view correspondingToItem:(NSUInteger)itemIdentifier;

//Sent when the list has scrolled to the bottom. By scrolled I mean: dragged, decellerated, or stopped--i.e. as soon as the view passes that point.
-(void)infiniteListViewScrolledPastBottom:(GBInfiniteListView *)infiniteListView;

//Sent when it scrolls. Same semantics as above. Measured as total offset of scrollable region (including header view, not including loading view)
-(void)infiniteListView:(GBInfiniteListView *)infiniteListView scrolledToPosition:(CGFloat)offset;

//Sent when the list of visible on screen items changes. (Only when it changes, so this doesn't get sent for every scroll, if you want that use infiniteListView:scrolledToPosition: and inside it call the itemsCurrentlyOnScreen method)
-(void)infiniteListView:(GBInfiniteListView *)infiniteListView listOfVisibleItemsChanged:(NSArray *)itemIdentifiers;

//When an item leaves the screen, this is called
-(void)infiniteListView:(GBInfiniteListView *)infiniteListView view:(UIView *)view correspondingToItemWentOffScreen:(NSUInteger)itemIdentifier;

//When an item is scrolled onto screen
-(void)infiniteListView:(GBInfiniteListView *)infiniteListView view:(UIView *)view correspondingToItemCameOnScreen:(NSUInteger)itemIdentifier;

//Started loading. Lets you connect UI stuff to this. You shouldn't use this for the data dance, the dataSource has his own delegate methods for this. This is just for UI related stuff, i.e. if you want to show an auxiliarry loading indicator
-(void)infiniteListViewDidStartLoadingMoreItems:(GBInfiniteListView *)infiniteListView;

//Sent as soon as you tell the view that the loading has completed. Same semantics as above. Don't use this for the data dance.
-(void)infiniteListViewDidFinishLoadingMoreItems:(GBInfiniteListView *)infiniteListView;

//Sent when the dataSource returns no to canLoadMoreItemsInInfiniteListView:
-(void)infiniteListViewNoMoreItemsAvailable:(GBInfiniteListView *)infiniteListView;

@end