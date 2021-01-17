#include "DesktopNotificationManagerCompat.h"
#include <NotificationActivationCallback.h>
#include <windows.ui.notifications.h>

using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::UI::Notifications;
using namespace Microsoft::WRL;




// The UUID CLSID must be unique to your app. Create a new GUID if copying this code.
class DECLSPEC_UUID("19a76fe0-7494-11d0-8816-00a0c903b83c") NotificationActivator WrlSealed WrlFinal
    : public RuntimeClass<RuntimeClassFlags<ClassicCom>, INotificationActivationCallback>
{
public:
    virtual HRESULT STDMETHODCALLTYPE Activate(
        _In_ LPCWSTR appUserModelId,
        _In_ LPCWSTR invokedArgs,
        _In_reads_(dataCount) const NOTIFICATION_USER_INPUT_DATA * data,
        ULONG dataCount) override
    {
        // TODO: Handle activation
        return 0;
    }
};

// Flag class as COM creatable
CoCreatableClass(NotificationActivator);




void Test2222()
{
    DesktopNotificationManagerCompat::RegisterAumidAndComServer(L"gfhfghtrf.edted", __uuidof(NotificationActivator));
    DesktopNotificationManagerCompat::RegisterActivator();
    ComPtr<IXmlDocument> doc;
    auto hr = DesktopNotificationManagerCompat::CreateXmlDocumentFromString(
        L"<toast><visual><binding template='ToastGeneric'><text>Hello world</text></binding></visual></toast>",
        &doc);
    if (SUCCEEDED(hr))
    {
        // See full code sample to learn how to inject dynamic text, buttons, and more

        // Create the notifier
        // Classic Win32 apps MUST use the compat method to create the notifier
        ComPtr<IToastNotifier> notifier;
        hr = DesktopNotificationManagerCompat::CreateToastNotifier(&notifier);
        if (SUCCEEDED(hr))
        {
            // Create the notification itself (using helper method from compat library)
            ComPtr<IToastNotification> toast;
            hr = DesktopNotificationManagerCompat::CreateToastNotification(doc.Get(), &toast);
            if (SUCCEEDED(hr))
            {
                // And show it!
                hr = notifier->Show(toast.Get());
            }
        }

    }
}



