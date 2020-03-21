#include "ntddk.h"


NTSTATUS RegistryCallback( PVOID CallbackContext,PVOID Argument1,PVOID Argument2);

VOID DriverUnload(PDRIVER_OBJECT pDrObj );


LARGE_INTEGER	g_CmCookies = {0};

UNICODE_STRING	g_AppInitValueName = {0};

NTSTATUS DriverEntry( PDRIVER_OBJECT pDrvObj , PUNICODE_STRING pRegistryPath)
{
	NTSTATUS nStatus = STATUS_UNSUCCESSFUL;
	do
	{
		UNREFERENCED_PARAMETER( pRegistryPath );
		//KdBreakPoint();
		RtlInitUnicodeString( &g_AppInitValueName , L"AppInit_DLLs");
		if( STATUS_SUCCESS  != CmRegisterCallback( RegistryCallback , NULL , &g_CmCookies) )
		{
			break;
		}
		pDrvObj->DriverUnload = DriverUnload;
		nStatus = TRUE;

	}
	while( FALSE );
	return nStatus;
}

VOID DriverUnload(PDRIVER_OBJECT pDrObj )
{
	UNREFERENCED_PARAMETER(pDrObj);
	CmUnRegisterCallback( g_CmCookies );
}

NTSTATUS RegistryCallback( PVOID CallbackContext,PVOID Argument1,PVOID Argument2)
{
	NTSTATUS nStatus = STATUS_SUCCESS;
	do
	{
		UNREFERENCED_PARAMETER( CallbackContext );
		if( ExGetPreviousMode() == KernelMode )
		{
			break;
		}
		switch( (REG_NOTIFY_CLASS)(ULONGLONG)Argument1 )
		{
			case RegNtPreQueryValueKey:
			{
				//TODO�������������ӶԽ������ֵĹ���
				REG_QUERY_VALUE_KEY_INFORMATION *pInfo = (REG_QUERY_VALUE_KEY_INFORMATION*)Argument2;
				if( pInfo == NULL )
				{
					break;
				}
				if( pInfo->ValueName == NULL || pInfo->ValueName->Buffer == NULL )
				{
					break;
				}
				if( 0 != RtlCompareUnicodeString( pInfo->ValueName , &g_AppInitValueName , TRUE) )
				{
					break;
				}
				//����Value����,ʵ���ϣ�������Ӧ��ͨ��pInfo->Object,���ObQueryNameString����ȫ·��
				//������Ϊ�ϸ�
				__try
				{
					*pInfo->ResultLength = 0;
				}
				except(EXCEPTION_EXECUTE_HANDLER )
				{

				}
				nStatus = STATUS_CALLBACK_BYPASS;
				break;
			}
			default:
			{
				break;
			}
		}
	}
	while( FALSE );
	return nStatus;
}
