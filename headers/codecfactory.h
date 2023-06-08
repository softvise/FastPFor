/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef CODECFACTORY_H_
#define CODECFACTORY_H_

#include "codecs.h"

#include <map>
#include <memory>                                   // std::shared_ptr
#include <string>
#include <vector>

namespace FastPFor {

typedef std::map<std::string, std::shared_ptr<IntegerCODEC> > CodecMap;


/**
 * This class is a convenience class to generate codecs quickly.
 * It cannot be used safely in a multithreaded context where
 * each thread should have a different IntegerCODEC.
 */
class CODECFactory {
public:
    static CodecMap scodecmap;

    // hacked for convenience
    static std::vector<std::shared_ptr<IntegerCODEC>> allSchemes();
    static std::vector<std::string> allNames();
    static std::shared_ptr<IntegerCODEC> & getFromName(std::string name);
};


// C++11 allows better than this, but neither Microsoft nor Intel support C++11 fully.
CodecMap initializefactory();

} // namespace FastPFor

#endif /* CODECFACTORY_H_ */
