#ifndef BUILDALLBUTTONCONSTANTS_H
#define BUILDALLBUTTONCONSTANTS_H

namespace BuildAllButton {
namespace Constants {

const char ACTION_ID[] = "BuildAllButton.Action";
const char MENU_ID[] = "BuildAllButton.Menu";

// Эти константы от projectexplorer,
// но почему-то определены не в h, а в projectexplorer.cpp
const char BUILDSESSION[]         = "ProjectExplorer.BuildSession";
const int  P_ACTION_BUILDPROJECT  = 80;

} // namespace BuildAllButton
} // namespace Constants

#endif // BUILDALLBUTTONCONSTANTS_H

