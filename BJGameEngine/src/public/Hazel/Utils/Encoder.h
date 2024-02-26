#include <system/LvReflection.h>
#include <engine/LvScene.h>

namespace User
{
	static Lv::LvTypeId s_type = Lv::LvReflection::GetTypeId<test>();
	
	
	NcChangeAlpha* NcChangeAlpha::SetChangeTime(Lv::Engine::LvGameEntity baseGameObject,float fLifeTime,float fChangeTime,float fFromMeshAlphaValue,float fToMeshAlphaValue)
	{
		 ncChangeAlpha = baseGameObject.AddComponent<NcChangeAlpha>();
		ncChangeAlpha->SetChangeTime(fLifeTime, fChangeTime, fFromMeshAlphaValue, fToMeshAlphaValue);
		return ncChangeAlpha;
	}
	
	
	void NcChangeAlpha::SetChangeTime(float fDelayTime,float fChangeTime,float fFromAlphaValue,float fToAlphaValue)
	{
		m_fDelayTime = fDelayTime;
		m_fChangeTime = fChangeTime;
		m_fFromAlphaValue = fFromAlphaValue;
		m_fToMeshValue = fToAlphaValue;
		if (IsActive(gameObject))
		{
			Start();
			Update();
		}
	}
	
	
	void NcChangeAlpha::Restart()
	{
		m_fStartTime = GetEngineTime();
		m_fStartChangeTime = 0;
		ChangeToAlpha(0);
	}
	
	
	void NcChangeAlpha::Awake()
	{
		m_fStartTime = 0;
		m_fStartChangeTime = 0;
	}
	
	
	void NcChangeAlpha::Start()
	{
		Restart();
	}
	
	
	void NcChangeAlpha::Update()
	{
		if (0 < m_fStartChangeTime)
		{
			if (0 < m_fChangeTime)
			{
				 fElapsedRate = ((GetEngineTime() - m_fStartChangeTime) / m_fChangeTime);
				if (1 < fElapsedRate)
				{
					fElapsedRate = 1;
					if (m_bAutoDeactive && m_fToMeshValue <= 0)
					{
						SetActiveRecursively(gameObject, false);
					}
				}
				ChangeToAlpha(fElapsedRate);
			}
			else
			{
				ChangeToAlpha(1);
			}
		}
		else
		{
			if (0 < m_fStartTime)
			{
				if (m_fStartTime + m_fDelayTime <= GetEngineTime())
				{
					StartChange();
				}
			}
		}
	}
	
	
	void NcChangeAlpha::StartChange()
	{
		m_fStartChangeTime = GetEngineTime();
	}
	
	
	void NcChangeAlpha::ChangeToAlpha(float fElapsedRate)
	{
		 fAlphaValue = Mathf.Lerp(m_fFromAlphaValue, m_fToMeshValue, fElapsedRate);
		if (m_TargetType == TARGET_TYPE::MeshColor)
		{
			 meshFilters;
			if (m_bRecursively)
			{
				meshFilters = transform.GetComponentsInChildren<MeshFilter>(true);
			}
			else
			{
				meshFilters = transform.GetComponents<MeshFilter>();
			}
			 color;
			for( n = 0; n < meshFilters.Length; n++)
			{
				 colors = meshFilters[n].mesh.colors;
				if (colors.Length == 0)
				{
					if (meshFilters[n].mesh.vertices.Length == 0)
					{
						NcSpriteFactory.CreateEmptyMesh(meshFilters[n]);
					}
					colors = new Color[meshFilters[n].mesh.vertices.Length];
					for( c = 0; c < colors.Length; c++)
						colors[c] = Color.white;
				}
				for( c = 0; c < colors.Length; c++)
				{
					color = colors[c];
					color.a = fAlphaValue;
					colors[c] = color;
				}
				meshFilters[n].mesh.colors = colors;
			}
		}
		else
		{
			 rens;
			if (m_bRecursively)
			{
				rens = transform.GetComponentsInChildren<Lv::Engine::LvRenderComponent>(true);
			}
			else
			{
				rens = transform.GetComponents<Lv::Engine::LvRenderComponent>();
			}
			for( n = 0; n < rens.Length; n++)
			{
				 ren = rens[n];
				 colName = GetMaterialColorName(ren.sharedMaterial);
				if (colName != nullptr)
				{
					 col = ren.material.GetColor(colName);
					col.a = fAlphaValue;
					ren.material.SetColor(colName, col);
				}
			}
		}
		if (fElapsedRate == 1 && fAlphaValue == 0)
		{
			SetActiveRecursively(gameObject, false);
		}
	}
	
	
	void NcChangeAlpha::OnUpdateEffectSpeed(float fSpeedRate,bool bRuntime)
	{
		m_fDelayTime /= fSpeedRate;
		m_fChangeTime /= fSpeedRate;
	}
	
	
	void NcChangeAlpha::OnSetReplayState()
	{
		OnSetReplayState();
		m_NcEffectInitBackup = new NcEffectInitBackup();
		if (m_TargetType == TARGET_TYPE::MeshColor)
		{
			m_NcEffectInitBackup.BackupMeshColor(gameObject, m_bRecursively);
		}
		else
		{
			m_NcEffectInitBackup.BackupMaterialColor(gameObject, m_bRecursively);
		}
	}
	
	
	void NcChangeAlpha::OnResetReplayStage(bool bClearOldParticle)
	{
		OnResetReplayStage(bClearOldParticle);
		m_fStartTime = GetEngineTime();
		m_fStartChangeTime = 0;
		if (m_NcEffectInitBackup != nullptr)
		{
			if (m_TargetType == TARGET_TYPE::MeshColor)
			{
				m_NcEffectInitBackup.RestoreMeshColor();
			}
			else
			{
				m_NcEffectInitBackup.RestoreMaterialColor();
			}
		}
	}
	
}
