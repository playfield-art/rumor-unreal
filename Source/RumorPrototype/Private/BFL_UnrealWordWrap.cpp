/* ****************************************************************************
 * Written for the Light-Wizzard, UnrealWordWrap is Unreal Engine Word Wrap
 * written in C++ for Blueprints, and is Free Unlicensed Open Source.
 * https://github.com/Light-Wizzard/UnrealWordWrap
*/
#include "BFL_UnrealWordWrap.h"
// ****************************************************************************
/// <summary>
/// Load Text File relative to ProjectContentDir
/// </summary>
/// <param name="InputFileName">Relative to ProjectContentDir Input File Name</param>
/// <param name="TextOut">Text Out</param>
/// <returns>true if success, false if fail</returns>
bool UBFL_UnrealWordWrap::LoadTxtFile(FString InputFileName, FString& TextOut)
{
    TextOut.Empty();
    if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*(FPaths::ProjectContentDir() + InputFileName)))
    {
        return FFileHelper::LoadFileToString(TextOut, *(FPaths::ProjectContentDir() + InputFileName));
    }
    // You should not get here: Debug
    FString ProjectDirectory = FPaths::ProjectContentDir();
    UE_LOG(LogTemp, Error, TEXT("LoadTxtFile FilePaths: ProjectContentDir: %s"), *ProjectDirectory)
        return false;
}
// ****************************************************************************
/// <summary>
/// Save Text File relative to ProjectContentDir
/// </summary>
/// <param name="OutputFileName">Relative to ProjectContentDir Output File Name</param>
/// <param name="SaveText">Save Text to output file</param>
/// <returns>true if success, false if fail</returns>
bool UBFL_UnrealWordWrap::SaveTxtFile(FString OutputFileName, FString SaveText)
{
    return FFileHelper::SaveStringToFile(SaveText, *(FPaths::ProjectContentDir() + OutputFileName));
}
// ****************************************************************************
/// <summary>
/// Is File Exist relative to ProjectContentDir 
/// </summary>
/// <param name="FileName">File Name</param>
/// <param name="absolutePath">Absolute Path</param>
/// <returns>true if success, false if fail</returns>
bool UBFL_UnrealWordWrap::IsFile(FString FileName, FString& absolutePath)
{
    FString ProjectDirectory = FPaths::ProjectDir();
    // Prepend ProjectContentDir
    if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*(FPaths::ProjectContentDir() + FileName)))
    {
        absolutePath = FPlatformFileManager::Get().GetPlatformFile().ConvertToAbsolutePathForExternalAppForRead(*(FPaths::ProjectContentDir() + FileName));
        return true;
    }
    // You should not get here: Debug it
    ProjectDirectory = FPaths::ProjectContentDir();
    UE_LOG(LogTemp, Error, TEXT("FilePaths: ProjectContentDir: %s"), *ProjectDirectory);
    return false;
}
// ****************************************************************************
/// <summary>
/// Wrap Text at given lenght inserting br tags
/// </summary>
/// <param name="InputText">Input Text</param>
/// <param name="WrapAt">Wrap At inserting br tag</param>
/// <param name="TabLen">Tab Lenght: 0 and -1 removes tabs</param>
/// <param name="TextOut">Text Out</param>
/// <returns>true if success, false if fail</returns>
bool UBFL_UnrealWordWrap::WrapTxt(FString InputText, int32 WrapAt, int32 TabLen, FString& TextOut)
{
    // Sanity Chech If text is less than the Wrap At then return the Text
    if (InputText.Len() < 1) { TextOut.Empty(); return false; }
    else if (InputText.Len() < WrapAt) { TextOut = InputText; return true; }
    // Line Feed String
    FString TheLineFeed;
    TheLineFeed.AppendChar('\n');
    // Set the Index
    int32 TheIndex = 0;
    int32 TheCounter, TheBackTrack, TheLastIndex = -1;
    FString TheInputText = "";
    // Set Text Out Empty to length of string
    TextOut.Empty();
    // We have to remove all but the ending Carriage Return in CRLF and deal with Tabs
    for (TheCounter = 0; TheCounter < InputText.Len(); TheCounter++)
    {
        // CRLF \r\n
        if (InputText[TheCounter] == '\r')
        {
            if (InputText.Len() >= TheCounter + 1) { if (InputText[TheCounter + 1] == '\n') { continue; } }
        }
        else if (InputText[TheCounter] == '\t' || InputText[TheCounter] == '\b' || InputText[TheCounter] == '\f')
        {
            // Remove Tabs if less than 1
            if (TabLen > 0)
            {
                for (int32 TheTabIndex = 0; TheTabIndex < TabLen; TheTabIndex++)
                {
                    TheInputText.AppendChar(' ');
                }
                continue;
            }
            else { continue; }
        }
        else
        {
            // Text Render only allows Text, Alpha Numberics, Space, and Punctions
            // This is to sanitize Word Processor imbeded characters
            // If you need more, add them here, but test them first
            if (isalnum(InputText[TheCounter]) || isspace(InputText[TheCounter]) || ispunct(InputText[TheCounter]))
            {
                TheInputText.AppendChar(InputText[TheCounter]);
            }
            else
            {
                // There a lot of characters that you do not see in documents
                UE_LOG(LogTemp, Error, TEXT("Removed this character: %s"), InputText[TheCounter]);
            }
        }
    } // end for (TheCounter
    bool IsFirstChar;
    // While we have input text
    while (TheIndex < TheInputText.Len())
    {
        IsFirstChar = true;
        // Working with the copy of the input string
        for (TheCounter = 1; TheCounter <= WrapAt; TheCounter++)
        {
            // Check to see if it is at the end
            if (TheInputText.Len() == TheIndex)
            {
                // Note I used LF \n in file, if you want to use this for LF, or CRLF,
                // then change HTML br tage to what you want, and append a LF
                if (BreakTxt(TextOut, TextOut))
                {
                    return true;
                }
                else { return false; }
            }
            // Check for Line Feed tag we inserted into the original string 
            // to reset the counter and add br
            if (TheInputText[TheIndex] == '\r')
            {
                /* ************************************************************
                *  We are converting all CRLF to LF, normally only the last
                *  character is a line feed, the Line Feeds get turned into the
                *  HTML tag <br> for Unreal Engine, we remove all Tabs or replace
                *  with spaces, codes that show up as a box becasue it does not understand it,
                *  we have a filter for this below.
                */
                TextOut.AppendChar('\n'); // Mark Line Feed \n
                // If the next character is another Line Feed, skip it, it is part of the CRLF
                if (TheInputText.Len() < TheIndex + 1)
                {
                    if (TheInputText[TheIndex + 1] == '\n')
                    {
                        TheCounter = 1; // Reset the Counter
                        continue;
                    }
                }
                TheCounter = 1; // Reset the Counter
            }
            else if (TheInputText[TheIndex] == '\n') // We removed any CR in an CRLF
            {
                TextOut.AppendChar('\n'); // Mark Line Feed \n
                TheCounter = 1; // Reset the Counter
            }
            else
            {
                char theCharacter = static_cast<unsigned char>(TheInputText[TheIndex]);
                // Filter out stuff you do not want, 11 is code 8203 a space with no width, ungly in UE
                // Generaly Characters are above 32, which is a space, 33 is an !, and so on
                // A is 65, 126 is an ~, so you will want to see what looks ugly
                if (theCharacter != 11)
                {
                    // Remap Characters that do not work in Unreal becasue they display as a box
                    // Switch from the fancy " to the one that you can see in UE
                    if (theCharacter == 28 || theCharacter == 29) { theCharacter = '"'; }
                    // Switch from the fancy ' to the one that you can see in UE
                    if (theCharacter == 24 || theCharacter == 25) { theCharacter = 39; }
                    if (IsFirstChar)
                    {
                        IsFirstChar = false;
                        if (theCharacter != ' ')
                        {
                            TextOut.AppendChar(theCharacter);
                        }
                    }
                    else
                    {
                        TextOut.AppendChar(theCharacter);
                    }
                }
            }
            TheIndex++; // Increament the Index
        } // end for (theCounter
        // We ran to the End of this Line
            // Set the Back Text
        int32 TheStart = TheIndex - WrapAt;
        if (TheStart < 0) { TheStart = 0; }
        FString TheBackText = TheInputText.Mid(TheStart, WrapAt);
        // Look for characters we can wrap at in this order
        TArray<FString> TheBreakItems;
        if (TheBackText.Contains(TheLineFeed)) { TheBreakItems.Add(TheLineFeed); }
        if (TheBackText.Contains(" ")) { TheBreakItems.Add(TEXT(" ")); }
        if (TheBackText.Contains(".")) { TheBreakItems.Add(TEXT(".")); }
        if (TheBackText.Contains(";")) { TheBreakItems.Add(TEXT(";")); }
        if (TheBackText.Contains("?")) { TheBreakItems.Add(TEXT("?")); }
        if (TheBackText.Contains("!")) { TheBreakItems.Add(TEXT("!")); }
        if (TheBackText.Contains(":")) { TheBreakItems.Add(TEXT(":")); }
        if (TheBackText.Contains("\\")) { TheBreakItems.Add(TEXT("\\")); }
        if (TheBackText.Contains("/")) { TheBreakItems.Add(TEXT("/")); }
        if (TheBackText.Contains("|")) { TheBreakItems.Add(TEXT("|")); }
        if (TheBackText.Contains("-")) { TheBreakItems.Add(TEXT("-")); }
        if (TheBackText.Contains("_")) { TheBreakItems.Add(TEXT("_")); }
        if (TheBackText.Contains("&")) { TheBreakItems.Add(TEXT("&")); }
        if (TheBackText.Contains("*")) { TheBreakItems.Add(TEXT("*")); }
        if (TheBackText.Contains("=")) { TheBreakItems.Add(TEXT("=")); }
        if (TheBackText.Contains("+")) { TheBreakItems.Add(TEXT("+")); }
        if (TheBackText.Contains("<")) { TheBreakItems.Add(TEXT("<")); }
        if (TheBackText.Contains(">")) { TheBreakItems.Add(TEXT(">")); }
        if (TheBackText.Contains("[")) { TheBreakItems.Add(TEXT("[")); }
        if (TheBackText.Contains("]")) { TheBreakItems.Add(TEXT("]")); }
        if (TheBackText.Contains("{")) { TheBreakItems.Add(TEXT("{")); }
        if (TheBackText.Contains("}")) { TheBreakItems.Add(TEXT("}")); }
        if (TheBackText.Contains("(")) { TheBreakItems.Add(TEXT("(")); }
        if (TheBackText.Contains(")")) { TheBreakItems.Add(TEXT(")")); }
        if (TheBackText.Contains("@")) { TheBreakItems.Add(TEXT("@")); }
        if (TheBackText.Contains("#")) { TheBreakItems.Add(TEXT("#")); }
        if (TheBackText.Contains("$")) { TheBreakItems.Add(TEXT("$")); }
        if (TheBackText.Contains("%")) { TheBreakItems.Add(TEXT("%")); }
        if (TheBackText.Contains("^")) { TheBreakItems.Add(TEXT("^")); }
        if (TheBackText.Contains("~")) { TheBreakItems.Add(TEXT("~")); }
        // The comman in a sentence is one thing, a number is another, so put at end
        if (TheBackText.Contains(",")) { TheBreakItems.Add(TEXT(",")); }
        // This is last, not nice to break on don't, but ok for 'LongLine'
        if (TheBackText.Contains("`")) { TheBreakItems.Add(TEXT("`")); }
        // Check for nearest space or other character in the list
        int32 TheBackline = 0;
        // We make TheChar an FString only the first character holds data
        FString TheChar;
        FString ThisChar;
        bool IsBraakable = false;
        int32 TheTextOutIndex;
        TheChar.Empty();
        TheChar.AppendChar(TheInputText[TheIndex]);
        // Compare Breakable character to input
        for (int32 i = 0; i < TheBreakItems.Num(); i++)
        {
            if (i > TheBreakItems.Num() - 1) { break; }
            if (TheChar.Compare(TheBreakItems[i]) == 0)
            {
                IsBraakable = true;
                break;
            }
        } // end for i
        // Check for IsBraakable
        if (IsBraakable)
        {
            TextOut.AppendChar('\n'); // Mark Line Feed
            TheCounter = 1; // Reset the Counter
        }
        else
        {
            for (TheBackTrack = TheIndex; TheBackTrack > 0; TheBackTrack--)
            {
                TheTextOutIndex = 0;
                IsBraakable = false;
                TheChar.Empty();
                TheChar.AppendChar(TheInputText[TheBackTrack]);
                // Compare Breakable character to input
                for (int32 i = 0; i < TheBreakItems.Num(); i++)
                {
                    if (i > TheBreakItems.Num() - 1) { break; }
                    if (TheChar.Compare(TheBreakItems[i]) == 0)
                    {
                        IsBraakable = true;
                        break;
                    }
                } // end for i
                // Is Breakable Character
                if (IsBraakable)
                {
                    int32 TheLen = TextOut.Len();
                    // TheBackTrack is the index of the InputText and not the same as TextOut,
                    // due to inserting New Line Feed, so it is off more every insert
                    // so we need to go backwards from the End of the string to find it
                    for (int32 ThisIndex = TheLen - 1; ThisIndex > 0; ThisIndex--)
                    {
                        ThisChar.Empty();
                        ThisChar.AppendChar(TextOut[ThisIndex]);
                        if (TheChar.Compare(ThisChar) == 0)
                        {
                            TheTextOutIndex = ThisIndex;
                            break;
                        }
                    } // end for
                    // Remove all characters going back to the breakable character
                    TextOut.RemoveAt(TheTextOutIndex + 1, TheLen - TheTextOutIndex);
                    TextOut.AppendChar('\n'); // Mark Line Feed
                    TheCounter = 1; // Reset the Counter
                    TheIndex = TheBackTrack + 1; // Reset the Index
                    break;
                } // end if (TheChar.Compare(
                TheBackline++;
            } // end for (TheBackTrack
        } // end if (isspace(TheInputText[theIndex]))
    } // end while (theIndex < TheInputText.Len())
    // We should not hit this code unless the size is too small
    if (BreakTxt(TextOut, TextOut))
    {
        return true;
    }
    else { return false; }
} // end WrapTxt
// ****************************************************************************
/// <summary>
/// Break Text
/// </summary>
/// <param name="InputText">Input Text</param>
/// <param name="TheTextOut">The Text Out</param>
/// <returns>true if success, false if fail</returns>
bool UBFL_UnrealWordWrap::BreakTxt(FString InputText, FString& TheTextOut)
{
    // Line Feed String
    FString TheLineFeed;
    TheLineFeed.AppendChar('\n');
    FString TheChar;
    TheTextOut.Empty();
    for (int32 TheCounter = 0; TheCounter < InputText.Len(); TheCounter++)
    {
        TheChar.Empty();
        TheChar.AppendChar(InputText[TheCounter]);
        if (TheChar.Compare(TheLineFeed) == 0) { TheTextOut.Append("<br>"); }
        else { TheTextOut.Append(TheChar); }
    }
    return (TheTextOut.Len() > 0);
}
// ***************************** End of File **********************************
