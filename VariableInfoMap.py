from VariableInfo import VariableInfo

defaultVarInfoMap = dict()

defaultVarInfoMap['TrueMuonCos'] = VariableInfo(
    Name = 'TrueMuonCos',
    Expr = 'TrueMuonCos',
    Latex = r'True $\cos{\theta_{\mu}}$',
    CustomBinning=[-1.0000, 0.1516, 0.5282, 0.7320, 0.8446, 0.9122, 0.9518, 0.9780, 1.0000],
    Unit = ''
)

defaultVarInfoMap['TrueMuonProtonCos'] = VariableInfo(
    Name = 'TrueMuonProtonCos',
    Expr = 'TrueMuonProtonCos',
    Latex = r'True $\cos{\theta_{\mu,p}}$',
    CustomBinning=[-1.0000, -0.5806, -0.3628, -0.1926, -0.0424, 0.0814, 0.2018, 0.3286, 0.4878, 0.7016, 1.0000],
    BinNormWidth=0.1,
    ytitle='events/(0.1)',
    Unit = '',
)

defaultVarInfoMap['TruedeltaPT'] = VariableInfo(
    Name = 'TruedeltaPT',
    Expr = 'TruedeltaPT',
    Latex = r'True $\delta P_{T}$',
    CustomBinning=[0., 0.08, 0.17, 0.25, 0.35, 0.55, 0.8],
    BinNormWidth=0.1,
    Unit = 'GeV/c'
)

defaultVarInfoMap['TruedeltaalphaT'] = VariableInfo(
    Name = 'TruedeltaalphaT',
    Expr = 'TruedeltaalphaT',
    Latex = r'True $\delta \alpha_{T}$',
    CustomBinning=[0, 25, 60, 100, 130, 160, 180],
    BinNormWidth=1.,
    Unit = 'degree',
)
