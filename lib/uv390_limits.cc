#include "uv390_limits.hh"
#include "channel.hh"
#include "radioid.hh"
#include "contact.hh"
#include "rxgrouplist.hh"
#include "scanlist.hh"
#include "zone.hh"
#include "gpssystem.hh"
#include "roaming.hh"


UV390Limits::UV390Limits(QObject *parent)
  : RadioLimits(false, parent)
{
  // Define limits for call-sign DB
  _hasCallSignDB          = true;
  _callSignDBImplemented  = false;
  _numCallSignDBEntries   = 122197;

  add("settings", new RadioLimitItem {
        { "introLine1", new RadioLimitString(-1, 10, RadioLimitString::Unicode) },
        { "introLine2", new RadioLimitString(-1, 10, RadioLimitString::Unicode) },
        { "micLevel", new RadioLimitUInt(1, 10) },
        { "speech", new RadioLimitIgnoredBool() },
        { "power", new RadioLimitEnum {
            unsigned(Channel::Power::Low),
            unsigned(Channel::Power::Mid),
            unsigned(Channel::Power::High) } },
        { "squlech", new RadioLimitUInt(0, 10) },
        { "vox", new RadioLimitUInt(0, 10) },
        { "tot", new RadioLimitUInt(0, -1) }
        /// @todo check default radio ID.
      } );

  /* Define limits for radio IDs. */
  add("radioIDs", new RadioLimitList{
        { DMRRadioID::staticMetaObject, 1, 1, new RadioLimitObject {
            {"name", new RadioLimitString(1, 16, RadioLimitString::Unicode) },
            {"id", new RadioLimitUInt(0, 16777215)}
          } }
      } );

  /* Define limits for contacts. */
  add("contacts", new RadioLimitList{
        { DigitalContact::staticMetaObject, 1, 10000, new RadioLimitObject {
            { "name", new RadioLimitString(1, 16, RadioLimitString::Unicode) },
            { "ring", new RadioLimitBool() },
            { "type", new RadioLimitEnum {
                (unsigned) DigitalContact::PrivateCall,
                (unsigned) DigitalContact::GroupCall,
                (unsigned) DigitalContact::AllCall
              }},
            { "number", new RadioLimitUInt(0, 16777215) }
          } },
        { DTMFContact::staticMetaObject, -1, -1, new RadioLimitIgnored() }
      } );

  /* Define limits for group lists. */
  add("groupLists", new RadioLimitList(
        RXGroupList::staticMetaObject, 1, 250, new RadioLimitObject {
          { "name", new RadioLimitString(1, 16, RadioLimitString::Unicode) },
          { "contacts", new RadioLimitRefList(1, 32, DigitalContact::staticMetaObject) }
        }) );

  /* Define limits for channel list. */
  add("channels", new RadioLimitList(
        Channel::staticMetaObject, 1, 3000,
        new RadioLimitObjects {
          { AnalogChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(1, 16, RadioLimitString::Unicode)},
              {"rxFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 480.}})},
              {"txFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 480.}})},
              {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}},
              {"timeout", new RadioLimitUInt(0, -1, std::numeric_limits<unsigned>::max())},
              {"scanlist", new RadioLimitObjRef(ScanList::staticMetaObject)},
              {"vox", new RadioLimitUInt(0, 10, std::numeric_limits<unsigned>::max())},
              {"rxOnly", new RadioLimitBool()},
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"admit", new RadioLimitEnum{
                 (unsigned)AnalogChannel::Admit::Always,
                 (unsigned)AnalogChannel::Admit::Free,
                 (unsigned)AnalogChannel::Admit::Tone
               } },
              {"squelch", new RadioLimitUInt(0, 10, std::numeric_limits<unsigned>::max())},
              {"bandwidth", new RadioLimitEnum{
                 (unsigned)AnalogChannel::Bandwidth::Narrow,
                 (unsigned)AnalogChannel::Bandwidth::Wide
               }},
              {"aprs", new RadioLimitObjRefIgnored(nullptr, RadioLimitIssue::Hint)}
            } },
          { DigitalChannel::staticMetaObject,
            new RadioLimitObject {
              {"name", new RadioLimitString(1, 16, RadioLimitString::Unicode)},
              {"rxFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 480.}})},
              {"txFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 480.}})},
              {"power", new RadioLimitEnum {
                 unsigned(Channel::Power::Low),
                 unsigned(Channel::Power::Mid),
                 unsigned(Channel::Power::High),
               }},
              {"timeout", new RadioLimitUInt(0, -1, std::numeric_limits<unsigned>::max())},
              {"scanlist", new RadioLimitObjRef(ScanList::staticMetaObject)},
              {"vox", new RadioLimitUInt(0, 10, std::numeric_limits<unsigned>::max())},
              {"rxOnly", new RadioLimitBool()},
              {"admit", new RadioLimitEnum {
                 unsigned(DigitalChannel::Admit::Always),
                 unsigned(DigitalChannel::Admit::Free),
                 unsigned(DigitalChannel::Admit::ColorCode) } },
              {"colorCode", new RadioLimitUInt(0,16)},
              {"timeSlot", new RadioLimitEnum {
                 unsigned(DigitalChannel::TimeSlot::TS1),
                 unsigned(DigitalChannel::TimeSlot::TS2) } },
              {"radioID", new RadioLimitObjRef(RadioID::staticMetaObject, true)},
              {"groupList", new RadioLimitObjRef(RXGroupList::staticMetaObject, false)},
              {"contact", new RadioLimitObjRef(DigitalContact::staticMetaObject, true)},
              {"aprs", new RadioLimitObjRefIgnored()},
              {"roaming", new RadioLimitObjRefIgnored(DefaultRoamingZone::get())},
              {"openGD77", new RadioLimitIgnored(RadioLimitIssue::Hint)},
              {"tyt", new RadioLimitIgnored(RadioLimitIssue::Hint)}
            } }
        } ) );

  /* Define limits for zone list. */
  add("zones", new RadioLimitList(
        Zone::staticMetaObject, 1, 250, new RadioLimitObject {
          { "name", new RadioLimitString(1, 16, RadioLimitString::Unicode) }, // 16 ASCII chars in name
          { "A", new RadioLimitRefList(0, 64, Channel::staticMetaObject) },
          { "B", new RadioLimitRefList(0, 64, Channel::staticMetaObject) },
          { "anytone", new RadioLimitIgnored(RadioLimitIssue::Hint) }     // ignore AnyTone extensions
        } ) );

  /* Define limits for scan lists. */
  add("scanlists", new RadioLimitList(
        ScanList::staticMetaObject, 1, 250, new RadioLimitObject{
          { "name", new RadioLimitString(1, 16, RadioLimitString::Unicode) },
          { "primary", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "secondary", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "revert", new RadioLimitObjRef(Channel::staticMetaObject, true) },
          { "channels", new RadioLimitRefList(0, 31, Channel::staticMetaObject) }
        }) );

  /* Define limits for positioning systems. */
  add("positioning", new RadioLimitList(
        GPSSystem::staticMetaObject, 0, 16, new RadioLimitObject {
          { "name", new RadioLimitStringIgnored() },
          { "period", new RadioLimitUInt(0, 7650) },
          { "contact", new RadioLimitObjRef(DigitalContact::staticMetaObject, false) },
          { "revert", new RadioLimitObjRef(DigitalChannel::staticMetaObject, true) }
        } ) );

  /* Ignore roaming zones. */
  add("roaming", new RadioLimitList(
        ConfigObject::staticMetaObject, -1, -1, new RadioLimitIgnored(RadioLimitIssue::Hint)
        ) );
}
