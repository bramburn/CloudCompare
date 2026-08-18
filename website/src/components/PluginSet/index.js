import clsx from 'clsx';
import Link from '@docusaurus/Link';
import Heading from '@theme/Heading';
import styles from './styles.module.css';

/**
 * The 18 self-contained plugins enabled in the local build. Data is
 * hard-coded (rather than loaded from a JSON file) because this is
 * rendered at build time on the homepage and we want the bundle to
 * stay as small as possible. The source of truth for this list is
 * the [`Local set`](/docs/plugins/local-set) doc page.
 */
const PLUGIN_GROUPS = [
  {
    type: 'I/O',
    description: 'File-format readers and writers',
    plugins: [
      {name: 'qCoreIO', what: 'OBJ · PLY · BIN · ASCII · E57 · PTX · PV · VTK · FBX (text)'},
    ],
  },
  {
    type: 'GL',
    description: 'OpenGL post-filters',
    plugins: [
      {name: 'qEDL', what: 'Eye Dome Lighting — makes dense clouds readable without normals'},
      {name: 'qSSAO', what: 'Screen-Space Ambient Occlusion for meshes'},
    ],
  },
  {
    type: 'Standard',
    description: 'Analysis & processing tools',
    plugins: [
      {name: 'qAnimation', what: 'Camera-path animation / flythrough'},
      {name: 'qBroom', what: 'Interactive cropping ("the broom")'},
      {name: 'qCSF', what: 'Cloth Simulation Filtering — ground / non-ground segmentation'},
      {name: 'qM3C2', what: 'Multiscale cloud-to-cloud distance'},
      {name: 'qPoissonRecon', what: 'Surface reconstruction from oriented points'},
      {name: 'qRANSAC_SD', what: 'Plane / sphere / cone / cylinder detection'},
      {name: 'qSRA', what: 'Surface Roughness Analysis'},
      {name: 'qHPR', what: 'Hidden Point Removal'},
      {name: 'qPCV', what: 'Principal Component View — turntable spin image'},
      {name: 'qColorimetricSegmenter', what: 'HSV / Lab colour-based segmentation'},
      {name: 'qMPlane', what: 'Manual plane definition'},
      {name: 'qVoxFall', what: 'Voxel fall-direction analysis'},
      {name: 'qCompass', what: 'Structural geology compass / strike-dip'},
      {name: 'qCanupo', what: 'CANUPO multi-scale classifier'},
      {name: '3DFin', what: '3D morphology / forestry biometrics'},
    ],
  },
];

const TYPE_VARIANT = {
  'I/O': 'io',
  GL: 'gl',
  Standard: 'std',
};

function PluginCard({name, what}) {
  return (
    <div className={styles.pluginCard}>
      <div className={styles.pluginName}>{name}</div>
      <div className={styles.pluginWhat}>{what}</div>
    </div>
  );
}

function PluginGroup({type, description, plugins}) {
  return (
    <div className={styles.group}>
      <div className={styles.groupHeader}>
        <span className={clsx(styles.typeBadge, styles[`type-${TYPE_VARIANT[type]}`])}>
          {type}
        </span>
        <span className={styles.groupDesc}>{description}</span>
        <span className={styles.groupCount}>
          {plugins.length} {plugins.length === 1 ? 'plugin' : 'plugins'}
        </span>
      </div>
      <div className={styles.pluginGrid}>
        {plugins.map((p) => (
          <PluginCard key={p.name} {...p} />
        ))}
      </div>
    </div>
  );
}

function PluginSet() {
  const total = PLUGIN_GROUPS.reduce((acc, g) => acc + g.plugins.length, 0);
  return (
    <section className={styles.pluginSet}>
      <div className="container">
        <div className={styles.header}>
          <span className={styles.eyebrow}>The local set</span>
          <Heading as="h2">{total} self-contained plugins</Heading>
          <p>
            The build is reproducible from a fresh clone with no extra
            dependencies beyond Qt 6. The heavyweight plugins (PCL, PDAL,
            FBX, OpenCASCADE) are off by default — they each have a vcpkg
            recipe on the{' '}
            <Link to="/docs/plugins/disabled-priority">disabled-priority</Link>{' '}
            page.
          </p>
        </div>
        <div className={styles.groups}>
          {PLUGIN_GROUPS.map((g) => (
            <PluginGroup key={g.type} {...g} />
          ))}
        </div>
        <div className={styles.cta}>
          <Link
            className="button button--secondary button--lg"
            to="/docs/plugins/local-set">
            See the full plugin table →
          </Link>
        </div>
      </div>
    </section>
  );
}

export default PluginSet;
