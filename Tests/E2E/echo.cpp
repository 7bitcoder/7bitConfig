#include <../../Include/SevenBit/Conf.hpp>
#include <iostream>

using namespace sb::cf;

int main(const int argc, char **argv)
{
    const IConfiguration::Ptr configuration = ConfigurationBuilder{} //
                                                  .addCommandLine(argc, argv)
                                                  .addEnvironmentVariables()
                                                  .build();
    std::cout << *configuration;
    return 0;
}
