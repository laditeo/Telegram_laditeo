// LADITEO
// Single switches for this fork. Defaults are off.
// Flip a flag to true and rebuild to restore the stock UI.
#pragma once

namespace Laditeo {

// Clock time drawn for a message (bubble, forwarded date, media overlay,
// chat list, call bubble, live-location clock, date tooltip).
inline constexpr bool kShowMessageTimes = false;

// "last seen" / "online" / "recently" presence text. Not an account privacy
// change: the client just does not draw it. Bots and service labels stay.
inline constexpr bool kShowLastSeen = false;

} // namespace Laditeo
