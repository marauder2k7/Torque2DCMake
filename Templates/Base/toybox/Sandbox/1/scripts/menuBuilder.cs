//-----------------------------------------------------------------------------
// Menu Builder Helper Class
//-----------------------------------------------------------------------------
/// @class MenuBuilder 
/// @brief Create Dynamic Context and MenuBar Menus
///
///
/// Summary : The MenuBuilder script class exists merely as a helper for creating
///           popup menu's for use in torque editors.  It is setup as a single 
///           object with dynamic fields starting with item[0]..[n] that describe
///           how to create the menu in question.  An example is below.
///
/// isPopup : isPopup is a persistent field on PopupMenu console class which
///           when specified to true will allow you to perform .showPopup(x,y) 
///           commands which allow popupmenu's to be used/reused as menubar menus
///           as well as context menus.
///
/// barPosition : barPosition indicates which index on the menu bar (0 = leftmost)
///           to place this menu if it is attached.  Use the attachToMenuBar() command
///           to attach a menu.
///
/// barName : barName specifies the visible name of a menu item that is attached
///           to the global menubar.
///
/// Remarks : If you wish to use a menu as a context popup menu, isPopup must be 
///           specified as true at the creation time of the menu.
///
/// 
/// @li @b item[n] (String) TAB (String) TAB (String) : <c>A Menu Item Definition.</c>
/// @code item[0] = "Open File..." TAB "Ctrl O" TAB "Something::OpenFile"; @endcode
///
/// @li @b IsPopup (bool) : <c>If Specified the menu will be considered a popup menu and should be used via .showPopup()</c>
/// @code IsPopup = true; @endcode
///
///
/// Example : Creating a @b MenuBar Menu
/// @code
/// %%editMenu = new PopupMenu()
/// {
///    barPosition = 3;
///    barName     = "View";
///    superClass = "MenuBuilder";
///    item[0] = "Undo" TAB "Ctrl Z" TAB "levelBuilderUndo(1);";
///    item[1] = "Redo" TAB "Ctrl Y" TAB "levelBuilderRedo(1);";
///    item[2] = "-";
/// };
///
/// %%editMenu.attachToMenuBar( 1, "Edit" );
///
/// @endcode
///
///
/// Example : Creating a @b Context (Popup) Menu
/// @code
/// %%contextMenu = new PopupMenu()
/// {
///    superClass = MenuBuilder;
///    isPopup    = true;
///    item[0] = "My Super Cool Item" TAB "Ctrl 2" TAB "echo(\"Clicked Super Cool Item\");";
///    item[1] = "-";
/// };
///
/// %%contextMenu.showPopup();
/// @endcode
///
///
/// Example : Modifying a Menu
/// @code
/// %%editMenu = new PopupMenu()
/// {
///    item[0] = "Foo" TAB "Ctrl F" TAB "echo(\"clicked Foo\")";
///    item[1] = "-";
/// };
/// %%editMenu.addItem( 2, "Bar" TAB "Ctrl B" TAB "echo(\"clicked Bar\")" );
/// %%editMenu.removeItem( 0 );
/// %%editMenu.addItem( 0, "Modified Foo" TAB "Ctrl F" TAB "echo(\"clicked modified Foo\")" );
/// @endcode
///
///
/// @see PopupMenu
///
//-----------------------------------------------------------------------------

// Adds one item to the menu.
// if %item is skipped or "", we will use %item[#], which was set when the menu was created.
// if %item is provided, then we update %item[#].
function MenuBuilder::addItem(%this, %pos, %item)
{
   if(%item $= "")
      %item = %this.item[%pos];
   
   if(%item !$= %this.item[%pos])
      %this.item[%pos] = %item;
   
   %name = getField(%item, 0);
   %accel = getField(%item, 1);
   %cmd = getField(%item, 2);
   
   if(isObject(%accel))
   {
      // If %accel is an object, we want to add a sub menu
      %this.insertSubmenu(%pos, %name, %accel);
   }
   else
   {      
      %this.insertItem(%pos, %name !$= "-" ? %name : "", %accel);
   }
}

function MenuBuilder::onAdd(%this)
{
   for(%i = 0;%this.item[%i] !$= "";%i++)
   {
      %this.addItem(%i);

      %name = getField(%this.item[%i], 0);
      %accel = getField(%this.item[%i], 1);
      %cmd = getField(%this.item[%i], 2);
      
      if(! isObject(%accel) && $platform !$= "macos" && %name !$= "-" && %accel !$= "")
         GlobalActionMap.bindCmd("keyboard", %accel, %this @ ".onSelectItem(" @ %i @ ", \"\");", "");
   }
}

function MenuBuilder::onRemove(%this)
{
   for(%i = 0;%this.item[%i] !$= "";%i++)
   {
      %name = getField(%this.item[%i], 0);
      %accel = getField(%this.item[%i], 1);
      %cmd = getField(%this.item[%i], 2);
      
      if(! isObject(%accel) && $platform !$= "macos" && %name !$= "-" && %accel !$= "")
         GlobalActionMap.unbind("keyboard", %accel);
   }
}

function MenuBuilder::onSelectItem(%this, %id, %text)
{
   %cmd = getField(%this.item[%id], 2);
   if(%cmd !$= "")
   {
      eval( %cmd );
      return true;
   }
   return false;
}

/// (SimID this)
/// Wraps the attachToMenuBar call so that it does not require knowledge of
/// barName or barIndex to be removed/attached.  This makes the individual 
/// MenuBuilder items very easy to add and remove dynamically from a bar.
///
function MenuBuilder::attachToMenuBar( %this )
{
   if( %this.barName $= "" )
   {
      error("MenuBuilder::attachToMenuBar - Menu property 'barName' not specified!");
      return false;
   }
   
   if( %this.barPosition < 0 )
   {
      error("MenuBuilder::attachToMenuBar - Menu " SPC %this.barName SPC "property 'barPosition' is invalid, must be greater than 0!");
      return false;
   }
   
   for(%i = 0;%this.item[%i] !$= "";%i++)
   {
      %name = getField(%this.item[%i], 0);
      %accel = getField(%this.item[%i], 1);
      %cmd = getField(%this.item[%i], 2);
      
      if(! isObject(%accel) && $platform !$= "macos" && %name !$= "-" && %accel !$= "")
         GlobalActionMap.bindCmd("keyboard", %accel, %this @ ".onSelectItem(" @ %i @ ", \"\");", "");
   }
   
   Parent::attachToMenuBar( %this, %this.barPosition, %this.barName );
   
}

/// (SimID this)
/// Wraps the removeToMenuBar call so that windows accels may be unbound
///
function MenuBuilder::removeFromMenuBar( %this )
{   
   for(%i = 0;%this.item[%i] !$= "";%i++)
   {
      %name = getField(%this.item[%i], 0);
      %accel = getField(%this.item[%i], 1);
      %cmd = getField(%this.item[%i], 2);
      
      if(! isObject(%accel) && $platform !$= "macos" && %name !$= "-" && %accel !$= "")
         GlobalActionMap.unbind("keyboard", %accel);
   }
   
   Parent::removeFromMenuBar( %this );
   
}
