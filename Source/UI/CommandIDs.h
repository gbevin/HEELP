/*
 * This file is part of HEELP.
 * Copyright (c) 2017 Uwyn SPRL.  http://www.uwyn.com
 *
 * HEELP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HEELP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef COMMANDIDS_H_INCLUDED
#define COMMANDIDS_H_INCLUDED

namespace heelp
{
    namespace CommandIDs
    {
        static const int showAbout                = 0x10001;
        static const int showPrefs                = 0x10002;
        
        static const int create                   = 0x20001;
        static const int open                     = 0x20002;
        static const int save                     = 0x20003;
        static const int saveAs                   = 0x20004;
        static const int showSettings             = 0x20005;
        static const int quit                     = 0x20006;
        
        static const int undo                     = 0x30001;
        static const int redo                     = 0x30002;
        
        static const int visitWebsite             = 0x50001;
        static const int reportIssue              = 0x50002;
    }

    namespace CommandCategories
    {
        static const char* const file    = "File";
        static const char* const edit    = "Edit";
        static const char* const view    = "View";
        static const char* const help    = "Help";
    }
}

#endif  // COMMANDIDS_H_INCLUDED
