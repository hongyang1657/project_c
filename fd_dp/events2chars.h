//
//  events2chars.h
//  AM
//
//  Created by Trilobite on 2019/4/8.
//  Copyright © 2019 Trilobite. All rights reserved.
//

#ifndef events2chars_h
#define events2chars_h

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <fstream>
#include "cJSON.h"
#include "Tool.h"

std::vector<std::string> events2chars(std::vector<std::string> events, bool special_intent_slot=true);

#endif /* events2chars_h */
