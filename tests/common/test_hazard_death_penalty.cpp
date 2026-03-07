#include "doctest.h"
#include "common/PlayerKillStyles.h"

TEST_CASE("Hazard deaths are not penalized when the option is off")
{
    CHECK_FALSE(ShouldPenalizeHazardDeath(Environment, false));
}

TEST_CASE("Hazard deaths are penalized when the option is on")
{
    CHECK(ShouldPenalizeHazardDeath(Environment, true));
}

TEST_CASE("Non-hazard deaths are still penalized even when the option is off")
{
    CHECK(ShouldPenalizeHazardDeath(Stomp, false));
    CHECK(ShouldPenalizeHazardDeath(Bomb, false));
}